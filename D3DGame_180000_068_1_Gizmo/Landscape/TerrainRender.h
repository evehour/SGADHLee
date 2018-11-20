#pragma once

class TerrainRender
{
public:
	TerrainRender();
	~TerrainRender();

	void Update();
	void Render();

	UINT Width() { return width; }
	UINT Height() { return height; }

	Texture* HeightMap() { return heightMap; }
	float TerrainRender::Y(D3DXVECTOR3 & position);

private:
	Texture* heightMap;

	Material* material;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer, *indexBuffer;
	VertexTextureNormal* vertices;
	UINT* indices;

	UINT width, height;
	UINT vertexCount, indexCount;
};