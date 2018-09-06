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
	void PostRender() {}
	void ResizeScreen() {}

private:
	Shader* shader;

private:
	UINT planetsCount;
	class Planet* planets;
};