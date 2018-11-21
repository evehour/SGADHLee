#pragma once
#include "Execute.h"

class TestShadow : public Execute
{
public:
	TestShadow();
	~TestShadow();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	UINT modelCount;

	class GameModel** models;
	Shader *shader;
	
	thread **myThread;
};