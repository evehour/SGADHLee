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

	vector<wstring> tFiles;
	tFiles.push_back(Textures + L"Floor.png");
	tFiles.push_back(Textures + L"Box.png");
	tFiles.push_back(Textures + L"Bricks.png");
	tFiles.push_back(Textures + L"Dirt.png");
	tFiles.push_back(Textures + L"Dirt2.png");
	tFiles.push_back(Textures + L"Dirt3.png");

	textureArray = new TextureArray(tFiles, 512, 512);

	samplerState = new SamplerState();
	samplerState->AddressU(D3D11_TEXTURE_ADDRESS_WRAP);
	samplerState->AddressV(D3D11_TEXTURE_ADDRESS_WRAP);
	samplerState->AddressW(D3D11_TEXTURE_ADDRESS_WRAP);
	samplerState->Filter(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	//samplerState->MaxLOD(0.0f);
}

TestInstancing::~TestInstancing()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(worldBuffer);

	SAFE_DELETE(shader);

	SAFE_DELETE(samplerState);
	SAFE_DELETE(textureArray);
}

void TestInstancing::Update()
{

}

void TestInstancing::PreRender()
{

}

void TestInstancing::Render()
{

	D3D11_FILTER filters[] = {
		D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
		D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT ,
		D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR ,
		D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT ,
		D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR ,
		D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT ,
		D3D11_FILTER_MIN_MAG_MIP_LINEAR ,
		D3D11_FILTER_ANISOTROPIC ,
		D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT ,
		D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR ,
		D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT ,
		D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR ,
		D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT ,
		D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR ,
		D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT ,
		D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR ,
		D3D11_FILTER_COMPARISON_ANISOTROPIC ,
		D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT,
		D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR ,
		D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT ,
		D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR ,
		D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT ,
		D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR ,
		D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT ,
		D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR ,
		D3D11_FILTER_MINIMUM_ANISOTROPIC ,
		D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT ,
		D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR ,
		D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT ,
		D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR ,
		D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT ,
		D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR ,
		D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT ,
		D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR ,
		D3D11_FILTER_MAXIMUM_ANISOTROPIC

	};

	UINT stride[2] = { sizeof(VertexTexture), sizeof(Vertex) };
	UINT offset[2] = { 0, 0 };
	ID3D11Buffer* pBuffer[2] = { vertexBuffer, instanceBuffer };

	D3D::GetDC()->IASetVertexBuffers(0, 2, pBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Render();
	//texture->SetShaderResource(10);
	ID3D11ShaderResourceView* srv;
	srv = textureArray->GetSRV();
	D3D::GetDC()->PSSetShaderResources(10, 1, &srv);

	samplerState->PSSetSamplers(10);

	worldBuffer->SetVSBuffer(1);

	D3D::GetDC()->DrawInstanced(6, 6, 0, 0);
}

void TestInstancing::PostRender()
{

}