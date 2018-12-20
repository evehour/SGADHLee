#pragma once
#include "Execute.h"

class TestPixelRate : public Execute
{
public:
	TestPixelRate();
	~TestPixelRate();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void PostRender2D() {};
	void ResizeScreen() {};

private:
	Texture* image;
	Render2D* render2D;
	RenderTargetView* renderTargetView;
};