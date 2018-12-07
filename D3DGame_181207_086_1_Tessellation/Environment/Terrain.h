#pragma once

class Terrain
{
public:
	friend class QuadTree;

public:
	Terrain(Material* material, wstring heightMap, float gridX = 1.0f, float gridY = 1.0f, UINT patchSize = 32);
	~Terrain();

	void Update();
	void Render();

private:
	void CreateData();
	void CreateNormal();
	void CreateBuffer();

private:
	Material* material;
	Texture* heightMap;

	UINT width, height;
	float gridX, gridY;
	UINT patchSize; // ���� ���� ���� (��: 16�� �ָ� 16x16���� �ڸ��ڴٴ� ��)

	VertexTextureNormal* vertices;

	UINT vertexCount;

	ID3D11Buffer* vertexBuffer;

	class QuadTree* root;
	class Frustum* frustum;

	D3DXMATRIX World;
};