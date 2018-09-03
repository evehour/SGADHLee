#pragma once
#include "Execute.h"

class TestSpace : public Execute
{
public:
	TestSpace(ExecuteValues* values);
	~TestSpace();

	void Update();
	void PreRender() {}
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	Shader* shader;
	D3DXVECTOR3 a;

private:
	UINT planetsCount;
	class Planet* planets[1000];
};