#pragma once
#include "Execute.h"

class TestTessTerrain : public Execute
{
public:
	TestTessTerrain();
	~TestTessTerrain();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	Material* material;
	class Terrain* terrain;
};