#include "stdafx.h"
#include "TestBillboard.h"
#include "../Objects/MeshPlane.h"


TestBillboard::TestBillboard(ExecuteValues * values)
	:Execute(values)
{
	worldBuffer = new WorldBuffer();

	shader = new Shader(Shaders + L"048_Billboard.hlsl");
	shader->CreateGS();

	planeShader = new Shader(Shaders + L"042_Plane.hlsl");
	plane = new MeshPlane();
	plane->Scale(10, 1, 10);
	plane->SetDiffuse(1, 1, 1, 1);
	plane->SetShader(planeShader);

	VertexSize vertices[1000];
	for (UINT i = 0; i < 1000; i++)
	{
		vertices[i].Size = D3DXVECTOR2(
			Math::Random(1.0f, 3.0f),
			Math::Random(1.0f, 3.0f)
		);

		vertices[i].Position = D3DXVECTOR3(
			Math::Random(-50.0f, 50.0f),
			vertices[i].Size.y * 0.5f,
			Math::Random(-50.0f, 50.0f)
		);
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexSize) * 1000;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->FillMode(D3D11_FILL_WIREFRAME);

	texture = new Texture(Textures + L"Tree.png");
}

TestBillboard::~TestBillboard()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(worldBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(rasterizerState[0]);
	SAFE_DELETE(rasterizerState[1]);
}

void TestBillboard::Update()
{
	plane->Update();
}

void TestBillboard::PreRender()
{

}

void TestBillboard::Render()
{
	UINT stride = sizeof(VertexSize);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	shader->Render();
	texture->SetShaderResource(10);

	values->ViewProjection->SetGSBuffer(0);
	worldBuffer->SetGSBuffer(1);

	//rasterizerState[1]->RSSetState();
	D3D::GetDC()->Draw(1000, 0);
	//rasterizerState[0]->RSSetState();

	D3D::GetDC()->GSSetShader(NULL, NULL, 0);


	plane->Render();
}

void TestBillboard::PostRender()
{

}


