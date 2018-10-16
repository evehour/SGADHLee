#include "stdafx.h"
#include "Sky.h"

Sky::Sky(ExecuteValues* values)
	: values(values)
	, realTime(false), theta(0), phi(0), domeCount(32)
{
	mieTarget = new RenderTarget(values, 128, 64);
	rayleighTarget = new RenderTarget(values, 128, 64);

	scatterShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Scattering", "PS_Scattering");
	targetShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Target", "PS_Target");

	worldBuffer = new WorldBuffer();

	GenerateSphere();
	GenerateQuad();
	
	rasterizeState[0] = new RasterizerState();
	rasterizeState[1] = new RasterizerState();
	rasterizeState[1]->CullMode(D3D11_CULL_NONE);

	depthStencilState[0] = new DepthStencilState();
	depthStencilState[1] = new DepthStencilState();
	depthStencilState[1]->DepthEnable(false);
	depthStencilState[1]->DepthWriteEnable(D3D11_DEPTH_WRITE_MASK_ZERO);
}

Sky::~Sky()
{
	SAFE_RELEASE(quadBuffer);
	SAFE_DELETE_ARRAY(quadVertices);

	for (UINT i = 0; i < 2; i++)
	{
		SAFE_DELETE(rasterizeState[i]);
		SAFE_DELETE(depthStencilState[i]);
	}

	SAFE_DELETE(worldBuffer);

	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(scatterShader);

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

	rayleighTarget->Set();



	mieTarget->Set();
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

	rasterizeState[1]->RSSetState();
	depthStencilState[1]->OMSetDepthStencilState();

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);

	depthStencilState[0]->OMSetDepthStencilState();
	rasterizeState[0]->RSSetState();
}

void Sky::PostRender()
{
}

void Sky::ResizeScreen()
{
}

void Sky::GenerateSphere()
{

	UINT latitude = domeCount / 2;
	UINT longitude = domeCount;

	vertexCount = longitude * latitude * 2;
	indexCount = (longitude - 1) * (latitude - 1) * 2 * 8;

	VertexTexture* vertices = new VertexTexture[vertexCount];

	UINT index = 0;
	for (UINT i = 0; i < longitude; i++)
	{
		float xz = 100.0f * (i / (longitude - 1.0f)) * Math::PI / 180.0f;
		for (UINT j = 0; j < latitude; j++)
		{
			float y = Math::PI * j / (latitude - 1);

			vertices[index].Position.x = sinf(xz) * cosf(y);
			vertices[index].Position.y = cosf(xz);
			vertices[index].Position.z = sinf(xz) * sinf(y);
			vertices[index].Position *= 10.0f; // 임의로 크기 키우려고

			vertices[index].Uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].Uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		}//for(i)
	}//for(j)

	for (UINT i = 0; i < longitude; i++)
	{
		float xz = 100.0f * (i / (longitude - 1.0f)) * Math::PI / 180.0f;
		for (UINT j = 0; j < latitude; j++)
		{
			float y = (Math::PI * 2.0f) * (Math::PI * j / (latitude - 1));

			vertices[index].Position.x = sinf(xz) * cosf(y);
			vertices[index].Position.y = cosf(xz);
			vertices[index].Position.z = sinf(xz) * sinf(y);
			vertices[index].Position *= 10.0f; // 임의로 크기 키우려고

			vertices[index].Uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].Uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		}//for(i)
	}//for(j)

	index = 0;
	UINT* indices = new UINT[indexCount * 3];
	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = i * latitude + j;
			indices[index++] = (i + 1) * latitude + j;
			indices[index++] = (i + 1) * latitude + j + 1;

			indices[index++] = (i + 1) * latitude + j + 1;
			indices[index++] = i * latitude + j + 1;
			indices[index++] = i * latitude + j;
		}
	}

	UINT offset = latitude * longitude;
	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = offset + i * latitude + j;
			indices[index++] = offset + (i + 1) * latitude + j + 1;
			indices[index++] = offset + (i + 1) * latitude + j;

			indices[index++] = offset + i * latitude + j + 1;
			indices[index++] = offset + (i + 1) * latitude + j + 1;
			indices[index++] = offset + i * latitude + j;
		}
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
	quadVertices[0].Uv = D3DXVECTOR2(1, 1);
	quadVertices[1].Uv = D3DXVECTOR2(0, 1);
	quadVertices[2].Uv = D3DXVECTOR2(0, 0);
	quadVertices[3].Uv = D3DXVECTOR2(0, 1);
	quadVertices[4].Uv = D3DXVECTOR2(1, 0);
	quadVertices[5].Uv = D3DXVECTOR2(1, 1);

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
