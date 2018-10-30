#pragma once
#include "Execute.h"

class TestShadow : public Execute
{
public:
	TestShadow(ExecuteValues* values);
	~TestShadow();

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

	class Shadow* shadow;
};