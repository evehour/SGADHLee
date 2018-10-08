#pragma once
#include "Execute.h"

class TestLandscape : public Execute
{
public:
	TestLandscape(ExecuteValues* values);
	~TestLandscape();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class Sky* sky;
	class TerrainRender* terrain;
	class Water* water;
};