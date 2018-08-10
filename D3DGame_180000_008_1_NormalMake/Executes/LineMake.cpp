#include <stdafx.h>
#include "./LineMake.h"

LineMake::LineMake(ExecuteValues * values)
	: Execute(values)
{
	shader = new Shader(Shaders + L"0080_Color.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer();
}

LineMake::~LineMake()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void LineMake::Update()
{
}

void LineMake::PreRender()
{
}

void LineMake::Render()
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	colorBuffer->SetPSBuffer(0);
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	for (int i = 0; i < indexCount / 2; i++)
		D3D::GetDC()->DrawIndexed(2, i * 2, 0);
}

void LineMake::PostRender()
{
}

void LineMake::ResizeScreen()
{
}

void LineMake::AddLine(D3DXVECTOR3 start, D3DXVECTOR3 end)
{
	m_lineList.push_back(make_pair(start, end));
}

void LineMake::ClearBuffer()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	m_lineList.clear();
}

void LineMake::UpdateBuffer()
{
	list<pair<D3DXVECTOR3, D3DXVECTOR3>>::iterator iter;
	int i = 0;

	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	vertexCount = m_lineList.size() * 2;
	indexCount = vertexCount;

	vertices = new VertexType[vertexCount];
	indices = new UINT[indexCount];

	for (iter = m_lineList.begin(); iter != m_lineList.end(); iter++, i++)
	{
		vertices[(i * 2) + 0].Position = iter->first;
		vertices[(i * 2) + 1].Position = iter->second;
		indices[(i * 2) + 0] = (i * 2);
		indices[(i * 2) + 1] = (i * 2) + 1;
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexType) * vertexCount;
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
}
