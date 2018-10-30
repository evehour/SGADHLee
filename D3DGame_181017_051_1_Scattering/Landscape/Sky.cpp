#include "stdafx.h"
#include "Sky.h"

Sky::Sky(ExecuteValues* values)
	: values(values)
	, realTime(false), theta(0), phi(0)
	, radius(10), slices(32), stacks(16)
{
	mieTarget = new RenderTarget(values, 128, 64);
	rayleighTarget = new RenderTarget(values, 128, 64);

	scatterShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Scattering", "PS_Scattering");
	targetShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Target", "PS_Target");

	worldBuffer = new WorldBuffer();

	GenerateSphere();
	GenerateQuad();

	depthStencilState[0] = new DepthStencilState();
	depthStencilState[1] = new DepthStencilState();
	depthStencilState[1]->DepthEnable(false);
	depthStencilState[1]->DepthWriteEnable(D3D11_DEPTH_WRITE_MASK_ZERO);

	rayleigh2D = new Render2D(values);
	rayleigh2D->Position(0, 100);
	rayleigh2D->Scale(200, 100);

	mie2D = new Render2D(values);
	mie2D->Position(0, 0);
	mie2D->Scale(200, 100);
}

Sky::~Sky()
{
	SAFE_RELEASE(quadBuffer);
	SAFE_DELETE_ARRAY(quadVertices);

	for (UINT i = 0; i < 2; i++)
	{
		SAFE_DELETE(depthStencilState[i]);
	}

	SAFE_DELETE(worldBuffer);

	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(scatterShader);
	SAFE_DELETE(targetShader);

	SAFE_DELETE(rayleighTarget);
	SAFE_DELETE(mieTarget);
}

void Sky::Update()
{
	D3DXMATRIX V;
	D3DXVECTOR3 pos;
	values->MainCamera->Position(&pos);
	D3DXMatrixTranslation(&V, pos.x, pos.y, pos.z);

	worldBuffer->SetMatrix(V);

}

void Sky::PreRender()
{
	if (prevTheta == theta && prevPhi == phi) return;

	mieTarget->Set();
	rayleighTarget->Set();

	ID3D11RenderTargetView* rtvs[2];
	rtvs[0] = rayleighTarget->GetRTV();
	rtvs[1] = mieTarget->GetRTV();

	ID3D11DepthStencilView* dsv;
	dsv = rayleighTarget->GetDSV();

	D3D::Get()->SetRenderTargets(2, rtvs, dsv);

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	targetShader->Render();

	D3D::GetDC()->Draw(6, 0);

}

void Sky::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	scatterShader->Render();

	worldBuffer->SetVSBuffer(1);

	depthStencilState[1]->OMSetDepthStencilState();

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);

	depthStencilState[0]->OMSetDepthStencilState();

	rayleigh2D->SRV(rayleighTarget->GetSRV());
	rayleigh2D->Update();
	rayleigh2D->Render();

	mie2D->SRV(mieTarget->GetSRV());
	mie2D->Update();
	mie2D->Render();
}

void Sky::PostRender()
{
}

void Sky::ResizeScreen()
{
}

void Sky::GenerateSphere()
{
	UINT radius = 10, slices = 32, stacks = 16;

	float sliceStep = (Math::PI * 2) / slices;
	float stackStep = (Math::PI) / stacks;

	///Create Vertex Data 
	vertexCount = (slices + 1) * (stacks + 1);
	VertexTexture* vertices = new VertexTexture[vertexCount];
	{
		float stackAngle = 0.0f; // - (Math::PI / 2);
		for (UINT y = 0; y <= stacks; y++) // 위도, altitude
		{
			float sliceAngle = 0.0f;
			for (UINT xz = 0; xz <= slices; xz++) // 경도, longitude
			{
				UINT index = (slices + 1) * y + xz;

				vertices[index].Position.x = (float)(radius * cosf(stackAngle) * cosf(sliceAngle));
				vertices[index].Position.y = (float)(radius * sinf(stackAngle));
				vertices[index].Position.z = (float)(radius * cosf(stackAngle) * sinf(sliceAngle));

				vertices[index].Uv.x = (1 / (float)slices) * xz;
				vertices[index].Uv.y = (1 / (float)stacks) * y;

				sliceAngle += sliceStep;
			}
			stackAngle += stackStep;
		}
	}

	///Create Index Data 
	indexCount = slices * stacks * 6;
	UINT* indices = new UINT[indexCount];
	{
		UINT index = 0;
		for (UINT y = 0; y < stacks; y++)
		{
			for (UINT x = 0; x < slices; x++)
			{
				indices[index + 0] = (slices + 1) * y + x; //0
				indices[index + 1] = (slices + 1) * (y + 1) + x; //1
				indices[index + 2] = (slices + 1) * y + (x + 1); //2

				indices[index + 3] = (slices + 1) * y + (x + 1); //2
				indices[index + 4] = (slices + 1) * (y + 1) + x; //1
				indices[index + 5] = (slices + 1) * (y + 1) + (x + 1); //3

				index += 6;
			}
		}//for(z)
	}


	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
}

void Sky::GenerateQuad()
{
	quadVertices = new VertexTexture[6];

	quadVertices[0].Position = D3DXVECTOR3(-1.0f, -1.0f, 0);
	quadVertices[1].Position = D3DXVECTOR3(-1.0f,  1.0f, 0);
	quadVertices[2].Position = D3DXVECTOR3( 1.0f, -1.0f, 0);
	quadVertices[3].Position = D3DXVECTOR3( 1.0f, -1.0f, 0);
	quadVertices[4].Position = D3DXVECTOR3(-1.0f,  1.0f, 0);
	quadVertices[5].Position = D3DXVECTOR3( 1.0f,  1.0f, 0);

	quadVertices[0].Uv = D3DXVECTOR2(0, 1); // 0
	quadVertices[1].Uv = D3DXVECTOR2(0, 0); // 1
	quadVertices[2].Uv = D3DXVECTOR2(1, 1); // 2
	quadVertices[3].Uv = D3DXVECTOR2(1, 1); // 2
	quadVertices[4].Uv = D3DXVECTOR2(0, 0); // 1
	quadVertices[5].Uv = D3DXVECTOR2(1, 0); // 3

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = quadVertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadBuffer);
		assert(SUCCEEDED(hr));
	}
}
