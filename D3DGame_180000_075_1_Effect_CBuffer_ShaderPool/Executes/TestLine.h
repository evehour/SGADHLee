#pragma once
#include "Execute.h"

class TestLine : public Execute
{
public:
	TestLine();
	~TestLine();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	ILine* bBox;
	DebugLine* boxLine;

	Material* cubeMaterial;
	MeshCube* cube;
};