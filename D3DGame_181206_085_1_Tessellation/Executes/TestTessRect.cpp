#include "stdafx.h"
#include "TestTessRect.h"

TestTessRect::TestTessRect()
{
	shader = new Shader(Shaders + L"059_TessRect.fx");

	Vertex vertices[4];
	vertices[0].Position = D3DXVECTOR3(-0.5f, -0.5f, 0);
	vertices[1].Position = D3DXVECTOR3(-0.5f, +0.5f, 0);
	vertices[2].Position = D3DXVECTOR3(+0.5f, -0.5f, 0);
	vertices[3].Position = D3DXVECTOR3(+0.5f, +0.5f, 0);

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}
	
	D3DXMatrixIdentity(&world);
	shader->AsMatrix("World")->SetMatrix(world);
}

TestTessRect::~TestTessRect()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(shader);
}

void TestTessRect::Update()
{

}

void TestTessRect::PreRender()
{

}

void TestTessRect::Render()
{
	static int amount = 1;
	static int amountInside = 1;

	if(ImGui::SliderInt("Amount", &amount, 1, 100))
		shader->AsScalar("TessAmount")->SetInt(amount);
	if (ImGui::SliderInt("AmountInside", &amountInside, 1, 100))
		shader->AsScalar("TessAmountInside")->SetInt(amountInside);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	
	shader->Draw(0, 0, 4);
}

void TestTessRect::PostRender()
{

}
