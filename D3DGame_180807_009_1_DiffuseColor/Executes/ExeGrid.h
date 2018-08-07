#pragma once
#include "Execute.h"

class ExeGrid : public Execute
{

private:
	typedef VertexTextureNormal VertexType;

public:
	ExeGrid(ExecuteValues* values);
	~ExeGrid();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	Material * material;
	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT width, height;
	UINT vertexCount, indexCount;

	VertexType* vertices;
	UINT* indices;

	Texture* texture[3];

private:
	void CreateNormal();
};