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

	float GetHeight(float positionX, float positionZ);

private:
	void CreateData();
	void CreateNormal();
	void CreateBuffer();

private:
	Material* material;
	Texture* heightMap;

	vector<D3DXCOLOR> heights;
	float weightY;

	UINT width, height;
	float gridX, gridY;
	UINT patchSize; // 최하 분할 갯수 (예: 16을 주면 16x16으로 자르겠다는 뜻)

	VertexTextureNormalIndex* vertices;

	UINT vertexCount;

	ID3D11Buffer* vertexBuffer;

	class QuadTree* root;
	class Frustum* frustum;

	D3DXMATRIX World;
	D3DXPLANE frustumPlanes[6];
};