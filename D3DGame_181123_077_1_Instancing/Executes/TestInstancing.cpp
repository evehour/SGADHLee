#include "stdafx.h"
#include "TestInstancing.h"

#include "./Environment/Terrain.h"


TestInstancing::TestInstancing()
{
	terrain = new Terrain();

	shader = new Shader(Shaders + L"054_Instance.fx");

	VertexTexture vertex[6];
	vertex[0].Position = D3DXVECTOR3(-0.5f, -0.5f, 0);
	vertex[1].Position = D3DXVECTOR3(-0.5f, 0.5f, 0);
	vertex[2].Position = D3DXVECTOR3(0.5f, -0.5f, 0);
	vertex[3].Position = D3DXVECTOR3(0.5f, -0.5f, 0);
	vertex[4].Position = D3DXVECTOR3(-0.5f, 0.5f, 0);
	vertex[5].Position = D3DXVECTOR3(0.5f, 0.5f, 0);

	vertex[0].Uv = D3DXVECTOR2(0, 1);
	vertex[1].Uv = D3DXVECTOR2(0, 0);
	vertex[2].Uv = D3DXVECTOR2(1, 1);
	vertex[3].Uv = D3DXVECTOR2(1, 1);
	vertex[4].Uv = D3DXVECTOR2(0, 0);
	vertex[5].Uv = D3DXVECTOR2(1, 0);

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertex;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[0]);
		assert(SUCCEEDED(hr));
	}

	VertexWorld vertexWorld[1000];
	for (UINT i = 0; i < 1000;i++)
	{
		D3DXVECTOR2 scale;
		scale = D3DXVECTOR2(
			Math::Random(1.0f, 3.0f),
			Math::Random(1.0f, 3.0f)
		);

		D3DXVECTOR3 position;

		position.x = Math::Random(0.0f, 255.0f);
		position.z = Math::Random(0.0f, 255.0f);
		position.y = terrain->GetHeight(position.x, position.z) + scale.y * 0.5f;

		D3DXMATRIX S, T;
		D3DXMatrixScaling(&S, scale.x, scale.y, 1);
		D3DXMatrixTranslation(&T, position.x, position.y, position.z);

		vertexWorld[i].World = S * T;
		vertexWorld[i].Texture = Math::Random(0, 5);
	}

	//Create Instance Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexWorld) * 1000;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertexWorld;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}

	vector<wstring> tFiles;
	tFiles.push_back(Textures + L"Floor.png");
	tFiles.push_back(Textures + L"Box.png");
	tFiles.push_back(Textures + L"Bricks.png");
	tFiles.push_back(Textures + L"Dirt.png");
	tFiles.push_back(Textures + L"Dirt2.png");
	tFiles.push_back(Textures + L"Dirt3.png");

	textureArray = new TextureArray(tFiles, 512, 512);
	shader->AsShaderResource("Map")->SetResource(textureArray->SRV());
}

TestInstancing::~TestInstancing()
{
	for (UINT i = 0; i < 2; i++)
		SAFE_RELEASE(vertexBuffer[i]);

	SAFE_DELETE(shader);

	SAFE_DELETE(textureArray);
	SAFE_DELETE(terrain);
}

void TestInstancing::Update()
{

}

void TestInstancing::PreRender()
{

}

void TestInstancing::Render()
{
	terrain->Render();

	UINT stride[2] = { sizeof(VertexTexture), sizeof(VertexWorld) };
	UINT offset[2] = { 0, 0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->DrawInstanced(0, 0, 6, 1000);
}

void TestInstancing::PostRender()
{

}