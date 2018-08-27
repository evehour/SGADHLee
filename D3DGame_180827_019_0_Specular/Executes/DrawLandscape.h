#pragma once
#include "Execute.h"

class DrawLandscape : public Execute
{
public:
	DrawLandscape();
	~DrawLandscape();

	void Update();
	void PreRender();
	void Render();
	void Resize();

private:
	class Sky* sky;
	class Terrain* terrain;
	class MeshSphere* sphere;
	vector<class Billboard *> trees;
};