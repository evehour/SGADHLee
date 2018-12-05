#pragma once
#include "Execute.h"

class TestBlur : public Execute
{
public:
	TestBlur();
	~TestBlur();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	class CubeSky* sky;

	wstring floor;
	wstring stone;
	wstring brick;

	class GameModel* bunny;

	Material* sphereMaterial;
	MeshSphere* sphere[10];

	Material* cylinderMaterial;
	MeshCylinder* cylinder[10];

	Material* boxMaterial;
	MeshCube* box;

	Material* planeMaterial;
	MeshGrid* plane;

	Material* quadMaterial;
	MeshQuad* quad;

	Viewport* viewport;
	RenderTargetView* rtv;
	DepthStencilView* dsv;

	Render2D* render2D;
};