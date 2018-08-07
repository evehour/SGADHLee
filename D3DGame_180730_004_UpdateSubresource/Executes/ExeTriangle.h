#pragma once
#include "Execute.h"

class ExeTriangle : public Execute
{
public:
	ExeTriangle(ExecuteValues* values);
	~ExeTriangle();

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

	UINT vertexCount, indexCount;

	Vertex* vertices;
	UINT* indices;
};