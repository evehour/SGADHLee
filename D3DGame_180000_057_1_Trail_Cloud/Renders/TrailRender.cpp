#include "stdafx.h"
#include "TrailRender.h"

TrailRender::TrailRender(int splitCount, wstring textureName)
	: splitCount(splitCount)
{
	deltaStoreTime = 0.0f;
	bViewTrail = false;

	trailShader = new Shader(Shaders + L"046_Trail.hlsl");
	trailTexture = new Texture(textureName);

	trailBuffer = new TrailBuffer();
	trailBuffer->Data.TrailCount = splitCount;

	//Create VertexBuffer
	{
		VertexTexture* vertices = new VertexTexture[6 * splitCount];
		float dx = 1.0f / (float)splitCount;

		for (UINT i = 0; i < (UINT)splitCount; i++)
		{
			//vertices[i * 6 + 0].Position = D3DXVECTOR3(0, -0.5f * 2, 0.0f);
			//vertices[i * 6 + 1].Position = D3DXVECTOR3(0,  0.5f * 2, 0.0f);
			//vertices[i * 6 + 2].Position = D3DXVECTOR3(0, -0.5f * 2, 0.0f);
			//vertices[i * 6 + 3].Position = D3DXVECTOR3(0, -0.5f * 2, 0.0f);
			//vertices[i * 6 + 4].Position = D3DXVECTOR3(0,  0.5f * 2, 0.0f);
			//vertices[i * 6 + 5].Position = D3DXVECTOR3(0,  0.5f * 2, 0.0f);

			//vertices[i * 6 + 0].Position = D3DXVECTOR3(-0.5f + (dx * (float)i), -0.5f, 0.0f);
			//vertices[i * 6 + 1].Position = D3DXVECTOR3(-0.5f + (dx * (float)i), 0.5f, 0.0f);
			//vertices[i * 6 + 2].Position = D3DXVECTOR3(-0.5f + dx * (float)(i + 1), -0.5f, 0.0f);
			//vertices[i * 6 + 3].Position = D3DXVECTOR3(-0.5f + dx * (float)(i + 1), -0.5f, 0.0f);
			//vertices[i * 6 + 4].Position = D3DXVECTOR3(-0.5f + (dx * (float)i), 0.5f, 0.0f);
			//vertices[i * 6 + 5].Position = D3DXVECTOR3(-0.5f + dx * (float)(i + 1), 0.5f, 0.0f);

			//vertices[i * 6 + 0].Position = D3DXVECTOR3(-0.5f * 2, 0, 0.0f);
			//vertices[i * 6 + 1].Position = D3DXVECTOR3( 0.5f * 2, 0, 0.0f);
			//vertices[i * 6 + 2].Position = D3DXVECTOR3(-0.5f * 2, 0, 0.0f);
			//vertices[i * 6 + 3].Position = D3DXVECTOR3(-0.5f * 2, 0, 0.0f);
			//vertices[i * 6 + 4].Position = D3DXVECTOR3( 0.5f * 2, 0, 0.0f);
			//vertices[i * 6 + 5].Position = D3DXVECTOR3( 0.5f * 2, 0, 0.0f);

			vertices[i * 6 + 0].Position = D3DXVECTOR3(0, 0, -0.5f * 2);
			vertices[i * 6 + 1].Position = D3DXVECTOR3(0, 0,  0.5f * 2);
			vertices[i * 6 + 2].Position = D3DXVECTOR3(0, 0, -0.5f * 2);
			vertices[i * 6 + 3].Position = D3DXVECTOR3(0, 0, -0.5f * 2);
			vertices[i * 6 + 4].Position = D3DXVECTOR3(0, 0,  0.5f * 2);
			vertices[i * 6 + 5].Position = D3DXVECTOR3(0, 0,  0.5f * 2);

			vertices[i * 6 + 0].Uv = D3DXVECTOR2((dx * (float)(i + 0)), 1);
			vertices[i * 6 + 1].Uv = D3DXVECTOR2((dx * (float)(i + 0)), 0);
			vertices[i * 6 + 2].Uv = D3DXVECTOR2((dx * (float)(i + 1)), 1);
			vertices[i * 6 + 3].Uv = D3DXVECTOR2((dx * (float)(i + 1)), 1);
			vertices[i * 6 + 4].Uv = D3DXVECTOR2((dx * (float)(i + 0)), 0);
			vertices[i * 6 + 5].Uv = D3DXVECTOR2((dx * (float)(i + 1)), 0);
		}

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = (sizeof(VertexTexture) * 6) * splitCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));

		SAFE_DELETE_ARRAY(vertices);
	}

	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->CullMode(D3D11_CULL_NONE);

	blendState[0] = new BlendState();
	blendState[1] = new BlendState();
	blendState[1]->BlendEnable(true);
	blendState[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendState[1]->DestBlend(D3D11_BLEND_ONE);
	blendState[1]->BlendOP(D3D11_BLEND_OP_ADD);
}

TrailRender::~TrailRender()
{
	for (UINT i = 0; i < 2; i++)
	{
		SAFE_DELETE(blendState[i]);
		SAFE_DELETE(rasterizerState[i]);
	}

	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(trailBuffer);
	SAFE_DELETE(trailTexture);
	SAFE_DELETE(trailShader);
}

void TrailRender::Update(D3DXMATRIX parent)
{
	deltaStoreTime += Time::Delta();

	if (deltaStoreTime >= 0.005f)
	{
		deltaStoreTime -= 0.005f;

		D3DXMatrixTranspose(&parent, &parent);
		SetAndShiftTrailMatBuffer(parent);
	}
}

void TrailRender::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11ShaderResourceView* view = trailTexture->GetView();
	D3D::GetDC()->PSSetShaderResources(10, 1, &view);

	trailBuffer->SetVSBuffer(10);


	rasterizerState[1]->RSSetState();
	blendState[1]->OMSetBlendState();
	trailShader->Render();
	D3D::GetDC()->Draw(6 * splitCount, 0);
	blendState[0]->OMSetBlendState();
	rasterizerState[0]->RSSetState();
}

void TrailRender::ClearTrail(D3DXMATRIX mat)
{
	D3DXMatrixTranspose(&mat, &mat);

	for (UINT i = 0; i < 128; i++)
		trailBuffer->Data.buffer[i] = mat;
}

void TrailRender::SetAndShiftTrailMatBuffer(D3DXMATRIX & insertFirstMat)
{
	memcpy
	(
		&trailBuffer->Data.buffer[1],
		&trailBuffer->Data.buffer[0],
		sizeof(D3DXMATRIX) * (128 - 1)
	);
	trailBuffer->Data.buffer[0] = insertFirstMat;
}
