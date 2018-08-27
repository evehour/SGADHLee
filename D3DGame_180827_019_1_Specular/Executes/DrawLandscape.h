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

	D3DXCOLOR diffuseColor;
	D3DXCOLOR specularColor;
	float shininess;

	class Sky* sky;
	class Terrain* terrain;
	class MeshSphere* sphere;
	vector<class Billboard*> trees;
};