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

private:
	void CreateData();
	void CreateNormalData();
	void CreateBuffer();

private:
	ExecuteValues* values;

	Material* material;
	WorldBuffer* worldBuffer;

	Texture* heightTexture;
	Texture* colorTexture;
	Texture* colorTexture2;
	Texture* alphaTexture;

	UINT width, height;

	VertexTextureNormal* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;

	ID3D11Buffer* vertexBuffer, *indexBuffer;

	ID3D11RasterizerState* rasterizer[2];
	ID3D11SamplerState* sampler;

private:
	class BrushBuffer : public ShaderBuffer
	{
	public:
		BrushBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Type = 0;
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

public:
	BrushBuffer* GetBrushBuffer() { return brushBuffer; }
};