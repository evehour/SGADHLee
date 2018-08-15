#pragma once
#pragma once
#include "Execute.h"

class DrawLandscape : public Execute
{
public:
	DrawLandscape(ExecuteValues* values);
	~DrawLandscape();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class Terrain* terrain;
	class MeshSphere* sphere;

public:
	Terrain* GetTerrain() { return terrain; }
};