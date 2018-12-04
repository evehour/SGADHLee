#pragma once
#include "Execute.h"

class TestCubeMap : public Execute
{
public:
	TestCubeMap();
	~TestCubeMap();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
};