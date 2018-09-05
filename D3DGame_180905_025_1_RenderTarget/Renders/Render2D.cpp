#include "stdafx.h"
#include "Render2D.h"

Render2D::Render2D(ExecuteValues * values)
{
	shader = new Shader(Shaders + L"990_Render2D.hlsl");
	worldBuffer = new WorldBuffer();

	//Create Ortho Matrix
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		D3DXMatrixOrthoOffCenterLH(&orthographic, 0, desc.Width, 0, desc.Height, -1, 1);
	}

	//Create DepthStencilState
	{
		depthState[0] = new DepthStencilState();
		depthState[1] = new DepthStencilState();
		depthState[1]->DepthEnable(false);
	}

	//Create VertexBuffer
	{

		VertexTexture* vertices = new VertexTexture[6];
		vertices[0].Position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);
		vertices[1].Position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
		vertices[2].Position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
		vertices[3].Position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
		vertices[4].Position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
		vertices[5].Position = D3DXVECTOR3(0.5f, 0.5f, 0.0f);

		vertices[0].Uv = D3DXVECTOR2(0, 1);
		vertices[0].Uv = D3DXVECTOR2(0, 0);
		vertices[0].Uv = D3DXVECTOR2(1, 1);
		vertices[0].Uv = D3DXVECTOR2(1, 1);
		vertices[0].Uv = D3DXVECTOR2(0, 0);
		vertices[0].Uv = D3DXVECTOR2(1, 1);

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));

		SAFE_DELETE_ARRAY(vertices);
	}
}

Render2D::~Render2D()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(depthState[0]);
	SAFE_DELETE(depthState[1]);

	SAFE_RELEASE(vertexBuffer);
}

void Render2D::Position(float x, float y)
{
}

void Render2D::Position(D3DXVECTOR2 vec)
{
}

void Render2D::Scale(float x, float y)
{
}

void Render2D::Scale(D3DXVECTOR2 vec)
{
}

void Render2D::Update()
{
}

void Render2D::Render()
{
}
