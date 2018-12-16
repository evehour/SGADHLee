#pragma once
#include "Execute.h"



class TestTessRectDistance : public Execute
{
public:
	TestTessRectDistance();
	~TestTessRectDistance();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void PostRender2D() {};
	void ResizeScreen() {};

private:
	Shader* shader;
	ID3D11Buffer* vertexBuffer;

	D3DXMATRIX world;
};