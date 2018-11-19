#pragma once
#include "Execute.h"

class TestGizmo : public Execute
{
public:
	TestGizmo(ExecuteValues* values);
	~TestGizmo();
	
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class MeshGrid* grid;
	Material* gridMaterial;

	struct Ray* ray;

	bool bPicking;

	class MeshCube* cube;
	Material* cubeMaterial;
	struct BBox* cubeBounding;
	class DebugLine* cubeLine;
	class GizmoComponent* gizmo;

private:
	vector<class GameRender*> entity;
};