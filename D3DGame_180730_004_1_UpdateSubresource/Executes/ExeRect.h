#pragma once
#include "Execute.h"

class ExeRect : public Execute
{

private:
	class ColorBuffer;

public:
	ExeRect(ExecuteValues* values);
	~ExeRect();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	int number;

	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT vertexCount, indexCount;

	VertexTexture* vertices;
	UINT* indices;

	ColorBuffer* colorBuffer;

	ID3D11ShaderResourceView* srv;			//텍스쳐를 그려주는 녀석

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