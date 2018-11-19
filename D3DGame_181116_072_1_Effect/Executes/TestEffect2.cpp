#include "stdafx.h"
#include "TestEffect2.h"


TestEffect2::TestEffect2()
{
	shader = new Shader(Shaders + L"052_effect2.fx");

	vertices = new Vertex[4];
	vertices[0].Position = D3DXVECTOR3(0, 0, 0);
	vertices[1].Position = D3DXVECTOR3(0, 1, 0);
	vertices[2].Position = D3DXVECTOR3(1, 0, 0);
	vertices[3].Position = D3DXVECTOR3(1, 1, 0);


	indices = new UINT[6]{ 0, 1, 2, 2, 1, 3 };


	//Create Vertex Buffer
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

	//Create Index Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

TestEffect2::~TestEffect2()
{
	SAFE_DELETE(shader);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
}

void TestEffect2::Update()
{
	D3DXMATRIX W;
	D3DXMatrixTranslation(&W, -1, 0, 0);

	shader->AsMatrix("World")->SetMatrix(W);
}

void TestEffect2::PreRender()
{

}

void TestEffect2::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Context::SetValues(shader);

	shader->IASetInputLayout(0, 0);
	shader->Apply(0, 0);

	D3D::GetDC()->DrawIndexed(6, 0, 0);
}

void TestEffect2::PostRender()
{

}


