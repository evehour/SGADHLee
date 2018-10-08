#pragma once
#include "Execute.h"

class TestBlend : public Execute
{
public:
	TestBlend(ExecuteValues* values);
	~TestBlend();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
};