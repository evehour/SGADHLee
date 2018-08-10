#include "stdafx.h"
#include "ExeGrid.h"

ExeGrid::ExeGrid(ExecuteValues * values)
	: Execute(values)
{
	Texture* heightMap = new Texture(Contents + L"HeightMaps/HeightMap_hw.png");
	vector<D3DXCOLOR> heights;
	heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);
	width = heightMap->GetWidth() - 1; // 정점의 갯수 때문에 1을 제외함.
	height = heightMap->GetHeight() - 1;


	shader = new Shader(Shaders + L"007_HeightsMap.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer();

	// Create VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VertexColor[vertexCount];

		// z축으로 하는 이유 3d라서
		for (UINT z = 0; z <= height; z++) {
			for (UINT x = 0; x <= width; x++) {
				UINT index = (width + 1) * z + x;

				vertices[index].Position.x = (float)x;
				vertices[index].Position.y = (float)(heights[index].a * 255.0f) / 7.5f;
				vertices[index].Position.z = (float)z;

				vertices[index].Color = D3DXCOLOR(1, 1, 1, 1);
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

	

	
	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertexCount;
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
		fillModeNumber = 0;
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);

		States::CreateRasterizer(&desc, &fillMode[0]);

		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &fillMode[1]);
	}
}

ExeGrid::~ExeGrid()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void ExeGrid::Update()
{
}

void ExeGrid::PreRender()
{
}

void ExeGrid::Render()
{
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->RSSetState(fillMode[fillModeNumber]);

	colorBuffer->SetPSBuffer(0);
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	
}
//IA -> Input Assembly		9
//VS -> Vertex Shader		9
/*
HS
TS		tessellation		11
DS
*/
//GS -> Geometry Shader		10
//----------------------------
//PS -> Pixel Shader  2D


void ExeGrid::PostRender()
{
	ImGui::SliderInt("WireFrame", &fillModeNumber, 0, 1);
}

void ExeGrid::ResizeScreen()
{
}
