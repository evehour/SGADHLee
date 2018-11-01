#pragma once
#include "./Execute.h"

class TestAnimaion : public Execute
{
public:
	TestAnimaion(ExecuteValues* values);
	~TestAnimaion();

	void Update();
	void PreRender() {}
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class BoneAnimationTool* animationTool;

	Shader* shader;
	class GameModel* model;
};