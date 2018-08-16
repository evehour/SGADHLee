#pragma once

class Terrain
{
public:
	Terrain(ExecuteValues* values, Material* material, wstring heightMap);
	~Terrain();

	void Update();
	void Render();

	float Y(D3DXVECTOR3& position);

	//2번째 방법
	bool Y(OUT D3DXVECTOR3* out, IN D3DXVECTOR3& position);

	bool Y2(OUT D3DXVECTOR3* out);

private:
	void CreateData();
	void CreateBuffer();

private:
	ExecuteValues* values;
	Material* material;
	WorldBuffer* worldBuffer;

	Texture* heightTexture;

	UINT width;
	UINT height;

	vector<VertexTextureNormal> vertices;
	vector<UINT> indices;

	ID3D11Buffer* vertexBuffer, *indexBuffer;

	ID3D11RasterizerState* rasterizer[2];
};