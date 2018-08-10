#pragma once
#include "Execute.h"

class Sphere : public Execute
{

private:
	class ColorBuffer;

public:
	Sphere(ExecuteValues* values);
	~Sphere();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	void SetPosition(D3DXVECTOR3 pos);

private:
	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT vertexCount, indexCount;

	VertexTextureNormal* vertices;
	UINT* indices;

	ColorBuffer* colorBuffer;

	ID3D11ShaderResourceView* srv;

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