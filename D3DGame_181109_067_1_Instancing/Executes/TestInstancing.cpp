#include "stdafx.h"
#include "TestInstancing.h"


TestInstancing::TestInstancing(ExecuteValues * values)
	:Execute(values)
{
	shader = new Shader(Shaders + L"051_Instance.hlsl");

	worldBuffer = new WorldBuffer();

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

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	Vertex instance[6];
	instance[0].Position = D3DXVECTOR3(-3.5f, -3.5f, 0);
	instance[1].Position = D3DXVECTOR3(-3.5f, +3.5f, 0);
	instance[2].Position = D3DXVECTOR3(+3.5f, -3.5f, 0);
	instance[3].Position = D3DXVECTOR3(+3.5f, -3.5f, 0);
	instance[4].Position = D3DXVECTOR3(+6.5f, -6.5f, 0);
	instance[5].Position = D3DXVECTOR3(+9.5f, -9.5f, 0);

	//Create Instance Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = instance;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &instanceBuffer);
		assert(SUCCEEDED(hr));
	}


	texture[0] = new Texture(Textures + L"Floor.png");
	texture[1] = new Texture(Textures + L"Box.png");
	texture[2] = new Texture(Textures + L"Bricks.png");
	texture[3] = new Texture(Textures + L"Dirt.png");
	texture[4] = new Texture(Textures + L"Dirt2.png");
	texture[5] = new Texture(Textures + L"Dirt3.png");
}

TestInstancing::~TestInstancing()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(worldBuffer);

	SAFE_DELETE(shader);

	for (int i = 0; i < 6;i++)
		SAFE_DELETE(texture[i]);
}

void TestInstancing::Update()
{

}

void TestInstancing::PreRender()
{

}

void TestInstancing::Render()
{
	UINT stride[2] = { sizeof(VertexTexture), sizeof(Vertex) };
	UINT offset[2] = { 0, 0 };
	ID3D11Buffer* pBuffer[2] = { vertexBuffer, instanceBuffer };

	D3D::GetDC()->IASetVertexBuffers(0, 2, pBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Render();
	//texture->SetShaderResource(10);
	ID3D11ShaderResourceView* srv[6];
	for (int i = 0; i < 6; i++)
		srv[i] = texture[i]->GetView();
	D3D::GetDC()->PSSetShaderResources(10, 6, srv);

	worldBuffer->SetVSBuffer(1);

	D3D::GetDC()->DrawInstanced(6, 6, 0, 0);
}

void TestInstancing::PostRender()
{

}