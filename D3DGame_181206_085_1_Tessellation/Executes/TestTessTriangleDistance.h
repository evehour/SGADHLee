#pragma once
#include "Execute.h"



class TestTessTriangleDistance : public Execute
{
public:
	TestTessTriangleDistance();
	~TestTessTriangleDistance();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	Shader* shader;
	ID3D11Buffer* vertexBuffer;

	D3DXMATRIX world;
};