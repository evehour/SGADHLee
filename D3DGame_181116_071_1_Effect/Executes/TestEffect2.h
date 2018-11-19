#pragma once
#include "Execute.h"

class TestEffect2 : public Execute
{
public:
	TestEffect2();
	~TestEffect2();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	Shader* shader;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	Vertex* vertices;
	UINT* indices;
};