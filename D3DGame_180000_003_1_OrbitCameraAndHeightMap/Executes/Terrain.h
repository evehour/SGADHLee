#pragma once
#include "Execute.h"

class Terrain : public Execute
{

private:
	class ColorBuffer;

public:
	Terrain(ExecuteValues* values);
	~Terrain();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	void GetCellPosition(int index, D3DXVECTOR3* pos);

private:
	Texture* heightMap;
	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT width, height;
	UINT vertexCount, indexCount;

	VertexTexture* vertices;
	UINT* indices;

	ColorBuffer* colorBuffer;

	int fillModeNumber;
	ID3D11RasterizerState* fillMode[2];

private:
	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 1, 1, 1);
		}
		~ColorBuffer()
		{

		}

		struct Struct
		{
			D3DXCOLOR Color;
		}Data;
	};
};