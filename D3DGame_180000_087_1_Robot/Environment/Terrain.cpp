#include "stdafx.h"
#include "Terrain.h"
#include "QuadTree.h"
#include "../Viewer/Frustum.h"

#include "../Viewer/Perspective.h"

Terrain::Terrain(Material * material, wstring heightMap, float gridX, float gridY, UINT patchSize)
	: material(material), gridX(gridX), gridY(gridY), patchSize(patchSize)
{
	this->heightMap = new Texture(heightMap);

	width = this->heightMap->GetWidth() - 1;
	height = this->heightMap->GetHeight() - 1;

	D3DXMatrixIdentity(&World);
	material->GetShader()->AsMatrix("World")->SetMatrix(World);

	CreateData();
	CreateNormal();
	CreateBuffer();

	frustum = new Frustum(1000);

	root = new QuadTree(this, frustum, material->GetShader());
	root->Build(D3DXVECTOR2((float)width / 2.0f, (float)height / 2.0f), (float)width); // 세로길이는 가로길이보다 크면 안됨... 왜안되냐면 그렇게 만들어서 ㅎㅎ
}

Terrain::~Terrain()
{
	SAFE_DELETE(frustum);

	SAFE_DELETE_ARRAY(vertices);

	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(heightMap);

	SAFE_DELETE(root);
}

void Terrain::Update()
{
	frustum->Update();
}

void Terrain::Render()
{
	UINT stride = sizeof(VertexTextureNormal);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	root->Render();
}


float Terrain::GetHeight(float positionX, float positionZ)
{
	UINT x = (UINT)positionX;
	UINT z = (UINT)positionZ;

	if (x < 0 || x >= width) return 0.0f;
	if (z < 0 || z >= height) return 0.0f;

	UINT index[4];
	index[0] = (width + 1) * z + x;
	index[1] = (width + 1) * (z + 1) + x;
	index[2] = (width + 1) * z + (x + 1);
	index[3] = (width + 1) * (z + 1) + x + 1;

	D3DXVECTOR3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = vertices[index[i]].Position;


	float ddx = (x - v[0].x) / 1.0f;
	float ddz = (z - v[0].z) / 1.0f;


	D3DXVECTOR3 temp;
	if (ddx + ddz <= 1)
	{
		temp = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	else
	{
		ddx = 1 - ddx;
		ddz = 1 - ddz;

		temp = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	return temp.y;
}

void Terrain::CreateData()
{

	vector<D3DXCOLOR> heights;
	this->heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	vertexCount = (width + 1) * (height + 1);
	vertices = new VertexTextureNormal[vertexCount];

	for (UINT z = 0; z <= height; z++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			UINT index = (width + 1) * z + x;

			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = (float)(heights[index].r * 255.0f) / 7.5f;
			vertices[index].Position.z = (float)z;

			vertices[index].Uv.x = (float)x / (float)width;
			vertices[index].Uv.y = 1.0f - ((float)z / (float)height); // v는 뒤집어야 한다고 함.
		}
	}//for(z)
}

void Terrain::CreateNormal()
{
	int index = 0;
	int idx[4] = { 0, 0, 0, 0 };
	int next = 0;
	D3DXVECTOR3 subtract[2];
	D3DXVECTOR3 temp(0, 0, 0);
	for (int z = 0; z <= height; z++)
	{
		for (int x = 0; x <= width; x++)
		{
			D3DXVECTOR3 normal(0, 0, 0);
			index = height * z + x;

			idx[0] = index + width;
			idx[1] = (x == width - 1) ? -1 : index + 1;
			idx[2] = index - width;
			idx[3] = (x == 0) ? -1 : index - 1;

			for (int i = 0; i < 4; i++)
			{
				if (idx[i] < 0 || idx[i] >= vertexCount) continue;
				next = (i + 1) % 4;
				if (idx[next] < 0 || idx[next] >= vertexCount) continue;

				subtract[0] = vertices[idx[i]].Position - vertices[index].Position;
				subtract[1] = vertices[idx[next]].Position - vertices[index].Position;
				D3DXVec3Cross(&temp, &subtract[0], &subtract[1]);

				normal += temp;
			}

			D3DXVec3Normalize(&vertices[index].Normal, &normal);
		}
	}
}

void Terrain::CreateBuffer()
{
	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormal) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}
}
