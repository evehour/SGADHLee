#include "stdafx.h"
#include "ExeTextureCoord.h"

ExeTextureCoord::ExeTextureCoord(ExecuteValues * values)
	: Execute(values)
	, vertexCount(4), indexCount(6)
	, number(0)
{
	shader = new Shader(Shaders + L"006_TextureUV.hlsl");
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


	//CreateSamplerState
	{
		D3D11_SAMPLER_DESC desc;
		States::GetSamplerDesc(&desc);

		States::CreateSampler(&desc, &state[0]);

		/*desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		States::CreateSampler(&desc, &state[1]);

		desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		States::CreateSampler(&desc, &state[2]);

		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		D3DXCOLOR color = D3DXCOLOR(1, 0, 0, 1);
		memcpy(&desc.BorderColor, &color, sizeof(D3DXCOLOR));*/

		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		States::CreateSampler(&desc, &state[1]);
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



	//Load SRV
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(D3D::GetDevice(), (Textures + L"Box.png").c_str(), NULL, NULL, &srv, NULL);
		assert(SUCCEEDED(hr));
	}
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(D3D::GetDevice(), (Textures + L"Wall.png").c_str(), NULL, NULL, &srv2, NULL);
		assert(SUCCEEDED(hr));
	}
}

ExeTextureCoord::~ExeTextureCoord()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void ExeTextureCoord::Update()
{
}

void ExeTextureCoord::PreRender()
{
}

void ExeTextureCoord::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	colorBuffer->SetPSBuffer(0);
	worldBuffer->SetVSBuffer(1);
	shader->Render();


	D3D::GetDC()->PSSetSamplers(0, 1, &state[number]);
	D3D::GetDC()->PSSetShaderResources(0, 1, &srv);
	D3D::GetDC()->PSSetShaderResources(1, 1, &srv2);

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


void ExeTextureCoord::PostRender()
{
	ImGui::Begin("Color");
	{
		ImGui::SliderInt("Number", &number, 0, 1);
		ImGui::SliderFloat3("Color", (float *)&colorBuffer->Data.Color, 0, 1);
	}
	ImGui::End();
}

void ExeTextureCoord::ResizeScreen()
{
}
