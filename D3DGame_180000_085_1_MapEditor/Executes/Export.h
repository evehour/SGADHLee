#pragma once
#include "Execute.h"

class Export : public Execute
{
public:
	Export();
	~Export();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void PostRender2D() {}
	void ResizeScreen();

private:
	
};