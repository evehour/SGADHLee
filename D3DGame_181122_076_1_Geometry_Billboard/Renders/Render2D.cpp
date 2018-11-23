#include "stdafx.h"
#include "Render2D.h"
#include "./Viewer/Freedom.h"
#include "./Viewer/Viewport.h"
#include "./Viewer/Perspective.h"

Render2D::Render2D(wstring shaderFile, string vsName, string psName)
	: position(0, 0), scale(1, 1)
{
	String::Replace(&shaderFile, Shaders, L"");

	wstring file = L"";
	if (shaderFile.length() > 1)
	{
		file = Shaders + shaderFile;
		shader = new Shader(file);
	}
	else
	{
		file = Shaders + L"990_Render2D.fx";
		shader = new Shader(file);
	}

	//Create Ortho Matrix
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		D3DXMatrixOrthoOffCenterLH(&orthographic, 0, desc.Width, 0, desc.Height, -1, 1);
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
		vertices[1].Uv = D3DXVECTOR2(0, 0);
		vertices[2].Uv = D3DXVECTOR2(1, 1);
		vertices[3].Uv = D3DXVECTOR2(1, 1);
		vertices[4].Uv = D3DXVECTOR2(0, 0);
		vertices[5].Uv = D3DXVECTOR2(1, 0);

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

	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0, 0, -1), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
}

Render2D::~Render2D()
{
	SAFE_DELETE(shader);

	SAFE_RELEASE(vertexBuffer);
}

void Render2D::Position(const float& x, const float& y)
{
	Position(D3DXVECTOR2(x, y));
}

void Render2D::Position(const D3DXVECTOR2& vec)
{
	position = vec;
}

void Render2D::Scale(const float& x, const float& y)
{
	Scale(D3DXVECTOR2(x, y));
}

void Render2D::Scale(const D3DXVECTOR2& vec)
{
	scale = vec;
}

void Render2D::Update()
{
	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, 1);
	D3DXMatrixTranslation(&T, position.x + scale.x * 0.5f, position.y + scale.y * 0.5f, 0);
	
	world = S * T;
}

void Render2D::Render()
{
	// 2D용 View, Projection 설정
	{
		shader->AsMatrix("World")->SetMatrix(world);
		shader->AsMatrix("View")->SetMatrix(view);
		shader->AsMatrix("Projection")->SetMatrix(orthographic);
	}

	// 드로잉  구간
	{
		UINT stride = sizeof(VertexTexture);
		UINT offset = 0;

		D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		shader->AsShaderResource("Map")->SetResource(srv);

		shader->Draw(0, 0, 6);

		ID3D11ShaderResourceView* pSRV = NULL;
		shader->AsShaderResource("Map")->SetResource(pSRV);
	}
}
