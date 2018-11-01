#pragma once
#include "Execute.h"

class TestDepth : public Execute
{
public:
	TestDepth(ExecuteValues* values);
	~TestDepth();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen(){}

private:
	Shader* shader;
	Shader* floorShader;
	Shader* depthShader;

	class Sky* sky;
	class MeshPlane* plane;
	class MeshSphere* sphere;
	class MeshCube* cube[2];

	RenderTarget* renderTarget[3];
	Render2D* postEffect;
};