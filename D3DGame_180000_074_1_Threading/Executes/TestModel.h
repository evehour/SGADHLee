#pragma once
#include "Execute.h"

class TestModel : public Execute
{
public:
	TestModel();
	~TestModel();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	UINT modelCount;

	//class GameModel* model;
	class GameModel** models;
	Shader *shader1, *shader2;
	
	thread **myThread;

	float freeVar;
};