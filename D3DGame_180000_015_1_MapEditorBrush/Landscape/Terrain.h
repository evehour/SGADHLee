#pragma once

class Terrain
{
public:
	Terrain(ExecuteValues* values, Material* material);
	~Terrain();

	void Update();
	void Render();

	float Y(D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out, D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out);
	void AdjustY(D3DXVECTOR3& location);
	void AdjustAlpha(D3DXVECTOR3& location);

private:
	void CreateData();
	void CreateNormalData();
	void CreateBuffer();
	void CreateColorData(UINT width, UINT height);
	void SaveAlphaMap(wstring fileName);

private:
	ExecuteValues* values;

	Material* material;
	WorldBuffer* worldBuffer;

	Texture* heightTexture;
	Texture* colorTexture;
	Texture* colorTexture2;
	Texture* colorTexture3;
	Texture* alphaTexture;

	UINT width, height;

	VertexColorTextureNormal* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;

	ID3D11Buffer* vertexBuffer, *indexBuffer;

	bool bWireFrame;
	RasterizerState* rasterizer[2];

private:
	class BrushBuffer : public ShaderBuffer
	{
	public:
		BrushBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Type = 1;
			Data.Location = D3DXVECTOR3(0, 0, 0);
			Data.Range = 2;
			Data.Color = D3DXVECTOR3(0, 1, 0);
		}

		struct Struct
		{
			int Type;
			D3DXVECTOR3 Location;

			int Range;
			D3DXVECTOR3 Color;
		} Data;
	};
	BrushBuffer* brushBuffer;

	vector<D3DXCOLOR> alphaColorBuffer;
	int brushMode, texNum;
	float strength;
	DXGI_FORMAT alphaFormat;
};