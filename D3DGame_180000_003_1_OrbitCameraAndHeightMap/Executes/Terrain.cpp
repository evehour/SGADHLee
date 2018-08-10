#include "stdafx.h"
#include "Terrain.h"

Terrain::Terrain(ExecuteValues * values)
	: Execute(values)
{
	heightMap = new Texture(Contents + L"HeightMaps/HeightMap_hw5.png");
	vector<D3DXCOLOR> heights;
	heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);
	width = heightMap->GetWidth() - 1; // ������ ���� ������ 1�� ������.
	height = heightMap->GetHeight() - 1;


	shader = new Shader(Shaders + L"004_Texture.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer();

	// Create VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VertexTexture[vertexCount];

		// z������ �ϴ� ���� 3d��
		for (UINT z = 0; z <= height; z++) {
			for (UINT x = 0; x <= width; x++) {
				UINT index = (vertexCount - 1) - ((width + 1) * z + x);

				vertices[index].Position.x = (float)x;
				vertices[index].Position.y = (float)(heights[index].a * 255.0f) / 7.5f;
				vertices[index].Position.z = (float)z;

				vertices[index].Uv = D3DXVECTOR2((float)(width - x) / (float)width, (float)(height - z) / (float)height);
			}
		} // for(z) 
	}

	// Create IndexData
	{
		indexCount = width * height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT z = 0; z < height; z++) {
			for (UINT x = 0; x < width; x++) {
				// 0�� index ���� �ϴ�
				indices[index + 0] = (width + 1) * z + x;
				// 1�� index ���� ���
				indices[index + 1] = (width + 1) * (z + 1) + x;
				// 2�� index ���� �ϴ�
				indices[index + 2] = (width + 1) * z + x + 1;

				// 2�� index ���� �ϴ�
				indices[index + 3] = (width + 1) * z + x + 1;
				// 1�� index ���� ���
				indices[index + 4] = (width + 1) * (z + 1) + x;
				// 3�� index ���� ���
				indices[index + 5] = (width + 1) * (z + 1) + x + 1;

				index += 6;
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
				// 0�� index ���� �ϴ�
				indices[index + 0] = (width + 1) * z + x;
				// 1�� index ���� ���
				indices[index + 1] = (width + 1) * (z + 1) + x;
				// 2�� index ���� �ϴ�
				indices[index + 2] = (width + 1) * z + x + 1;

				// 2�� index ���� �ϴ�
				indices[index + 3] = (width + 1) * z + x + 1;
				// 1�� index ���� ���
				indices[index + 4] = (width + 1) * (z + 1) + x;
				// 3�� index ���� ���
				indices[index + 5] = (width + 1) * (z + 1) + x + 1;

				index += 6;
			}
		} // for(z)
	}

	

	
	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount;
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

Terrain::~Terrain()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	SAFE_DELETE(heightMap);
}

void Terrain::Update()
{
}

void Terrain::PreRender()
{
}

void Terrain::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->RSSetState(fillMode[fillModeNumber]);

	colorBuffer->SetPSBuffer(0);
	worldBuffer->SetVSBuffer(1);
	heightMap->SetShaderResource(0);
	shader->Render();

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	
}

void Terrain::PostRender()
{
	ImGui::SliderInt("WireFrame", &fillModeNumber, 0, 1);
}

void Terrain::ResizeScreen()
{
}

void Terrain::GetCellPosition(int index, D3DXVECTOR3 * pos)
{
	*pos = vertices[index].Position;
}
