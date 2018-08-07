#include "stdafx.h"
#include "ExeGrid.h"

ExeGrid::ExeGrid(ExecuteValues * values)
	: Execute(values)
{
	Texture* heightMap = new Texture(Contents + L"HeightMaps/HeightMap.png");
	vector<D3DXCOLOR> heights;
	heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);
	width = heightMap->GetWidth() - 1; // 정점의 갯수 때문에 1을 제외함.
	height = heightMap->GetHeight() - 1;

	material = new Material(Shaders + L"009_1_Diffuse.hlsl");
	material->SetDiffuseMap(Contents + L"Textures/Dirt.png");


	worldBuffer = new WorldBuffer();

	texture[0] = new Texture(Contents + L"Textures/Wall.png");
	texture[1] = new Texture(Contents + L"HeightMaps/AlphaMap.png");

	// Create VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VertexType[vertexCount];

		// z축으로 하는 이유 3d라서 
		for (UINT z = 0; z <= height; z++) {
			for (UINT x = 0; x <= width; x++) {
				UINT index = (width + 1) * z + x;

				vertices[index].Position.x = (float)x;
				vertices[index].Position.y = (float)(heights[index].r * 255.0f) / 7.5f;
				vertices[index].Position.z = (float)z;

				vertices[index].Uv = D3DXVECTOR2((float)x / width, (float)z / height);
			}
		} // for(z) 
	}

	//vertices[14].Position.y = 5.0f;

	// Create IndexData
	{
		indexCount = width * height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT z = 0; z < height; z++) {
			for (UINT x = 0; x < width; x++) {
				// 0번 index 좌측 하단
				indices[index + 0] = (width + 1) * z + x;
				// 1번 index 좌측 상단
				indices[index + 1] = (width + 1) * (z + 1) + x;
				// 2번 index 우측 하단
				indices[index + 2] = (width + 1) * z + x + 1;

				// 2번 index 우측 하단
				indices[index + 3] = (width + 1) * z + x + 1;
				// 1번 index 좌측 상단
				indices[index + 4] = (width + 1) * (z + 1) + x;
				// 3번 index 좌측 상단
				indices[index + 5] = (width + 1) * (z + 1) + x + 1;

				index += 6;
			}
		} // for(z)
	}

	CreateNormal();

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

	{
		/*fillModeNumber = 0;
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);

		States::CreateRasterizer(&desc, &fillMode[0]);

		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &fillMode[1]);*/
	}
}

ExeGrid::~ExeGrid()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(material);

	for (int i = 0; i < 2; i++)
		SAFE_DELETE(texture[i]);

	SAFE_DELETE(worldBuffer);
}

void ExeGrid::Update()
{
}

void ExeGrid::PreRender()
{
}

void ExeGrid::Render()
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	texture[0]->SetShaderResource(1);
	texture[1]->SetShaderResource(2);

	worldBuffer->SetVSBuffer(1);
	material->PSSetBuffer();

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	
}

void ExeGrid::PostRender()
{
	ImGui::SliderFloat3("Diffuse", (float *)material->GetDiffuse(), 0, 1);
}

void ExeGrid::ResizeScreen()
{
}

void ExeGrid::CreateNormal()
{
	for (UINT i = 0; i < (indexCount / 3); i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VertexType v0 = vertices[index0];
		VertexType v1 = vertices[index1];
		VertexType v2 = vertices[index2];

		D3DXVECTOR3 d1 = v1.Position - v0.Position;
		D3DXVECTOR3 d2 = v2.Position - v0.Position;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &d1, &d2);

		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
	{
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
	}
}
