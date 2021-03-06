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
	class Shader* specular;

	class MeshSphere* pickObj;
	D3DXCOLOR diffuseColor;
	D3DXCOLOR specularColor;
	float shininess;

	class Sky* sky;
	class Terrain* terrain;

	vector<class MeshSphere *> spheres;
	vector<class Billboard *> trees;
};