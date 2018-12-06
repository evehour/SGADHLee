#pragma once
#include "Execute.h"



class TestTessRect : public Execute
{
public:
	TestTessRect();
	~TestTessRect();

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