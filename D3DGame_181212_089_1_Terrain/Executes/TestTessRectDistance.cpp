#include "stdafx.h"
#include "TestTessRectDistance.h"

TestTessRectDistance::TestTessRectDistance()
{
	shader = new Shader(Shaders + L"059_TessRectDist.fx");

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
	D3DXMatrixScaling(&world, 10, 10, 10);
	shader->AsMatrix("World")->SetMatrix(world);
}

TestTessRectDistance::~TestTessRectDistance()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(shader);
}

void TestTessRectDistance::Update()
{

}

void TestTessRectDistance::PreRender()
{

}

void TestTessRectDistance::Render()
{	
	static float ratio = 20;
	static float piece = 10;
	if (ImGui::SliderFloat("Ratio", &ratio, 1, 100))
		shader->AsScalar("Ratio")->SetFloat(ratio);
	if (ImGui::SliderFloat("Piece", &piece, 1, 64))
		shader->AsScalar("Piece")->SetFloat(piece);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	
	shader->Draw(0, 0, 4);
}

void TestTessRectDistance::PostRender()
{

}
