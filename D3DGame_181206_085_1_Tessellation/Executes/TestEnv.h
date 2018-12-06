#pragma once
#include "Execute.h"



class TestEnv : public Execute
{
public:
	TestEnv();
	~TestEnv();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	void BuildGeometry();

private:
	class CubeSky* sky;

	wstring floor;
	wstring stone;
	wstring brick;

	class GameModel* bunny;

	Material* sphereMaterial;
	MeshSphere* sphere[10];

	MeshSphere* sphereAround;
	float aroundDegree;

	Material* cylinderMaterial;
	MeshCylinder* cylinder[10];

	Material* boxMaterial;
	MeshCube* box;

	Material* planeMaterial;
	MeshGrid* plane;

	Material* quadMaterial;
	MeshQuad* quad;

	Viewport* viewport;
	DepthStencilView* dsv;

	RenderTargetView* rtv;
	RenderTargetView* rtv2;
	Render2D* render2D;
	Render2D* render2D2;

	class Blur* blur;
};