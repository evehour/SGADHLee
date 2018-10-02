#include <stdafx.h>
#include "./LineMake.h"

LineMake::LineMake()
	: vertexBuffer(NULL), indexBuffer(NULL)
	, vertices(NULL), indices(NULL)
{
	shader = new Shader(L"003_Color_Line");
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
	if (vertexBuffer)
	{
		UINT stride = sizeof(VertexType);
		UINT offset = 0;

		D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		colorBuffer->SetPSBuffer(10);
		worldBuffer->SetVSBuffer(1);
		shader->Render();

		for (UINT i = 0; i < indexCount / 2; i++)
			D3D::GetDC()->DrawIndexed(2, i * 2, 0);
	}
}

void LineMake::PostRender()
{
}

void LineMake::ResizeScreen()
{
}

void LineMake::MeshData(vector<ModelVertexType> vertices, vector<UINT> indices, UINT objNum)
{
	ClearBuffer();
	for (UINT i = 0; i < indices.size(); i += 3)
	{
		AddLine(vertices[indices[i + 0]].Position, vertices[indices[i + 1]].Position);
		AddLine(vertices[indices[i + 1]].Position, vertices[indices[i + 2]].Position);
		AddLine(vertices[indices[i + 2]].Position, vertices[indices[i + 0]].Position);
	}
	UpdateBuffer();
}

void LineMake::MeshData(vector<VertexTextureNormal> vertices, vector<UINT> indices, UINT objNum)
{
	ClearBuffer();
	for (UINT i = 0; i < indices.size(); i += 2)
	{
		AddLine(vertices[indices[i + 0]].Position, vertices[indices[i + 1]].Position);
	}
	UpdateBuffer();
}

void LineMake::MeshData(vector<D3DXVECTOR3> vertices, vector<UINT> indices, UINT objNum)
{
	ClearBuffer();

	switch (objNum)
	{
	case 0:
	case 1:
		for (UINT i = 0; i < indices.size() / 3; i++)
		{
			if (objNum == 0)
			{
				//AddLine(vertices[indices[(i * 3) + 0]], vertices[indices[(i * 3) + 1]]);
				//AddLine(vertices[indices[(i * 3) + 1]], vertices[indices[(i * 3) + 2]]);
				AddLine(vertices[indices[(i * 3) + 2]], vertices[indices[(i * 3) + 0]]);
			}
			else if(objNum == 1)
			{
				AddLine(vertices[indices[(i * 3) + 0]], vertices[indices[(i * 3) + 1]]);
				//AddLine(vertices[indices[(i * 3) + 1]], vertices[indices[(i * 3) + 2]]);
				AddLine(vertices[indices[(i * 3) + 2]], vertices[indices[(i * 3) + 0]]);
			}
		}
		break;
	case 2:
		// InputBuffer·Î ´ëÃ¼
		break;
	}
	UpdateBuffer();
}

void LineMake::MakeAABB(const D3DXVECTOR3 & minPos, const D3DXVECTOR3 & maxPos)
{
	ClearBuffer();
	AddLine(D3DXVECTOR3(minPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, minPos.y, minPos.z));
	AddLine(D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z));
	AddLine(D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), D3DXVECTOR3(minPos.x, minPos.y, maxPos.z));
	AddLine(D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), D3DXVECTOR3(minPos.x, minPos.y, minPos.z));

	AddLine(D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z));
	AddLine(D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z));
	AddLine(D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z));
	AddLine(D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), D3DXVECTOR3(minPos.x, maxPos.y, minPos.z));

	AddLine(D3DXVECTOR3(minPos.x, minPos.y, minPos.z), D3DXVECTOR3(minPos.x, maxPos.y, minPos.z));
	AddLine(D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z));
	AddLine(D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z));
	AddLine(D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z));
	UpdateBuffer();
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

void LineMake::InputBuffer(vector<VertexType> vertices, vector<UINT> indices)
{
	int i = 0;

	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(this->indices);
	SAFE_DELETE_ARRAY(this->vertices);

	vertexCount = vertices.size();
	indexCount = indices.size();

	this->vertices = new VertexType[vertexCount];
	this->indices = new UINT[indexCount];

	memcpy_s(&this->vertices[0], sizeof(VertexType) * vertexCount, &vertices[0], sizeof(VertexType) * vertexCount);
	memcpy_s(&this->indices[0], sizeof(UINT) * indexCount, &indices[0], sizeof(UINT) * indexCount);

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexType) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = this->vertices;

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
		data.pSysMem = this->indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void LineMake::SetColor(D3DXCOLOR& color)
{
	colorBuffer->Data.Color = color;
}

void LineMake::SetPosition(D3DXVECTOR3 & pos)
{
	D3DXMATRIX m = matWorld;
	m._41 = pos.x;
	m._42 = pos.y;
	m._43 = pos.z;

	SetWorld(m);
}

void LineMake::SetWorld(D3DXMATRIX & mat)
{
	matWorld = mat;
	worldBuffer->SetMatrix(matWorld);
}
