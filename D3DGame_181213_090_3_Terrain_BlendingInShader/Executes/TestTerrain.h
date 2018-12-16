#pragma once
#include "Execute.h"

class TestTerrain : public Execute
{
public:
	TestTerrain();
	~TestTerrain();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void PostRender2D() {};
	void ResizeScreen() {};

private:
	class CubeSky* sky;
	class Terrain* terrain;
};