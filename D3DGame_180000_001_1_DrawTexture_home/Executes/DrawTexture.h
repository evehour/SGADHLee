#pragma once
#include "Execute.h"

enum FACE_NUMBER
{
	FACE_NUMBER_FRONT = 0,
	FACE_NUMBER_LEFT,
	FACE_NUMBER_RIGHT,
	FACE_NUMBER_BACK,
	FACE_NUMBER_UP,
	FACE_NUMBER_DOWN,
	FACE_NUMBER_MAX
};

class DrawTexture : public Execute
{
private:
	class ColorBuffer;
	function<void(FACE_NUMBER, int, wstring)> m_fFileName[2][6];

public:
	DrawTexture(ExecuteValues* values);
	~DrawTexture();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	void SetTextureFileName(FACE_NUMBER faceNumber, int textureNumber, wstring fileName);

private:
	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT vertexCount, indexCount;

	VertexTexture* vertices;
	UINT* indices;

	ColorBuffer* colorBuffer;

	ID3D11ShaderResourceView* srv[2][6] = { NULL };			//텍스쳐를 그려주는 녀석

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