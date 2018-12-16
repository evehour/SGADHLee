#pragma once
#include "Execute.h"



class TestTessTriangle : public Execute
{
public:
	TestTessTriangle();
	~TestTessTriangle();

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