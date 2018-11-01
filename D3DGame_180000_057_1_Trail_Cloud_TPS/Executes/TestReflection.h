#pragma once
#include "Execute.h"

class TestReflection : public Execute
{
public:
	TestReflection(ExecuteValues* values);
	~TestReflection();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class GameAnimModel* hanjo;
	Shader* modelShader;

	class MeshPlane* plane;
	Shader* objectShader;

	class Reflection* reflection;
};