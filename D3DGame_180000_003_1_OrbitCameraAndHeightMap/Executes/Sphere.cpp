#include "stdafx.h"
#include "Sphere.h"
#include <fstream>

Sphere::Sphere(ExecuteValues * values)
	: Execute(values)
{
	shader = new Shader(Shaders + L"0071_TextureUVNormal.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer();
	//colorBuffer->Data.Color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	{
		ifstream fin;
		char input;

		fin.open(Contents + L"Model/sphere.txt");
		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}

		fin >> vertexCount;
		indexCount = vertexCount;

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin.get(input);
		fin.get(input);

		vertices = new VertexTextureNormal[vertexCount];
		indices = new UINT[indexCount];

		for (int i = 0; i < vertexCount; i++)
		{
			fin >> vertices[i].Position.x >> vertices[i].Position.y >> vertices[i].Position.z;
			fin >> vertices[i].Uv.x >> vertices[i].Uv.y;
			fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
			indices[i] = i;
		}
		fin.close();
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormal) * vertexCount;
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

	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(D3D::GetDevice(), (Textures + L"Box.png").c_str(), NULL, NULL, &srv, NULL);
		assert(SUCCEEDED(hr));
	}
}

Sphere::~Sphere()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);
	SAFE_RELEASE(srv);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void Sphere::Update()
{
}

void Sphere::PreRender()
{
}

void Sphere::Render()
{
	UINT stride = sizeof(VertexTextureNormal);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	colorBuffer->SetPSBuffer(0);
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	D3D::GetDC()->PSSetShaderResources(0, 1, &srv);
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	
}

void Sphere::PostRender()
{
	
}

void Sphere::ResizeScreen()
{
}

void Sphere::SetPosition(D3DXVECTOR3 pos)
{
	D3DXMATRIX matWorld;
	D3DXMatrixTranslation(&matWorld, pos.x, pos.y, pos.z);
	worldBuffer->SetMatrix(matWorld);
}
