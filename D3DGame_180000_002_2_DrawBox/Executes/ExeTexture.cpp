#include "stdafx.h"
#include "ExeTexture.h"

ExeTexture::ExeTexture(ExecuteValues * values)
	: Execute(values)
	, vertexCount(4), indexCount(6)
{
	shader = new Shader(Shaders + L"004_Texture.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer();

	vertices = new VertexTexture[vertexCount];
	vertices[0].Position = D3DXVECTOR3(0, 0, 0);
	vertices[1].Position = D3DXVECTOR3(0, 1, 0);
	vertices[2].Position = D3DXVECTOR3(1, 0, 0);
	vertices[3].Position = D3DXVECTOR3(1, 1, 0);


	vertices[0].Uv = D3DXVECTOR2(0, 1);
	vertices[1].Uv = D3DXVECTOR2(0, 0);
	vertices[2].Uv = D3DXVECTOR2(1, 1);
	vertices[3].Uv = D3DXVECTOR2(1, 0);

	indices = new UINT[indexCount]{ 0, 1, 2 , 2, 1, 3};

	
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

	//Load SRV
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(D3D::GetDevice(), (Textures + L"Box.png").c_str(), NULL, NULL, &srv, NULL);
		assert(SUCCEEDED(hr));
	}
}

ExeTexture::~ExeTexture()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void ExeTexture::Update()
{
}

void ExeTexture::PreRender()
{
}

void ExeTexture::Render()
{
	UINT stride = sizeof(VertexTexture);
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
//IA -> Input Assembly		9
//VS -> Vertex Shader		9
/*
HS
TS		tessellation		11
DS
*/
//GS -> Geometry Shader		10
//----------------------------
//PS -> Pixel Shader  2D


void ExeTexture::PostRender()
{
	ImGui::Begin("Color");
	{
		ImGui::SliderFloat3("Color", (float *)&colorBuffer->Data.Color, 0, 1);
	}
	ImGui::End();
}

void ExeTexture::ResizeScreen()
{
}
