#pragma once
#include "Execute.h"

class DrawTexture : public Execute
{

private:
	class ColorBuffer;
	class HalfBuffer;
	D3DXVECTOR2 half;
public:
	DrawTexture(ExecuteValues* values, D3DXVECTOR3 initPos);
	~DrawTexture();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	void SetWorldBuffer(D3DXMATRIX matWorld);
	void SetUV(D3DXVECTOR2 uv);
	void SetTextureFileName1(wstring fileName);
	void SetTextureFileName2(wstring fileName);

private:
	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT vertexCount, indexCount;

	VertexTexture* vertices;
	UINT* indices;

	ColorBuffer* colorBuffer;
	HalfBuffer* halfBuffer;

	ID3D11ShaderResourceView *srv1 = NULL, *srv2 = NULL;			//텍스쳐를 그려주는 녀석

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


	class HalfBuffer : public ShaderBuffer
	{
	public:
		HalfBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.half = D3DXVECTOR2(0.5f, 1);
			Data.pedding = D3DXVECTOR2(0, 0);
		}
		~HalfBuffer()
		{

		}

		struct Struct
		{
			D3DXVECTOR2 half;
			D3DXVECTOR2 pedding;
		}Data;
	};
};