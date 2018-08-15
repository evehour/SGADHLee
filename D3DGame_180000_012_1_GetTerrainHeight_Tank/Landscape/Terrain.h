#pragma once

class Terrain
{
public:
	Terrain(Material* material, wstring heightMap, D3DXVECTOR3& scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	~Terrain();

	void Update();
	void Render();

	//ù��° ���.
	float Y(D3DXVECTOR3& position);
	float Y(IN D3DXVECTOR3& position, OUT D3DXVECTOR3& right, D3DXVECTOR3& up, D3DXVECTOR3& forward);

	// �ι�° ���.
	// ������ ��� ���� �ݺ������� �˻��ؾ������� ������ ���.
	bool Y(OUT D3DXVECTOR3* out, IN D3DXVECTOR3& position);

private:
	void CreateData();
	void CreateBuffer();

private:
	Material* material;
	WorldBuffer* worldBuffer;

	Texture* heightTexture;

	UINT width;
	UINT height;
	D3DXVECTOR3 scale;

	vector<VertexTextureNormal> vertices;
	vector<UINT> indices;

	ID3D11Buffer* vertexBuffer, *indexBuffer;
};