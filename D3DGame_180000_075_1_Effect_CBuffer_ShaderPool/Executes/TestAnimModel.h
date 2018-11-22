#pragma once
#include "Execute.h"

class TestAnimModel : public Execute
{
public:
	TestAnimModel();
	~TestAnimModel();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	UINT modelCount;
	GameAnimModel** kachujin;

private:
	thread **myThread;
};