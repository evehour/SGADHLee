#pragma once
#include "Execute.h"

class ExeGrid : public Execute
{

private:
	class ColorBuffer;

public:
	ExeGrid(ExecuteValues* values);
	~ExeGrid();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT width, height;
	UINT vertexCount, indexCount;

	VertexColor* vertices;
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