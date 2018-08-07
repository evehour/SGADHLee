#pragma once
#include "Execute.h"

class ExeGrid : public Execute
{

private:
	typedef VertexTextureNormal VertexType;
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

	VertexType* vertices;
	UINT* indices;

	ColorBuffer* colorBuffer;

	Texture* texture[3];

private:
	void CreateNormal();

private:
	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Light = D3DXVECTOR3(-1, -1, -1);
		}
		~ColorBuffer()
		{

		}

		struct Struct
		{
			D3DXVECTOR3 Light;
			float padding;
		} Data;
	};
};