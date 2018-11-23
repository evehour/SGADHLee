#pragma once
#include "Execute.h"

class TestRenderTarget : public Execute
{
public:
	TestRenderTarget();
	~TestRenderTarget();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	class RenderTarget* renderTarget;
	class Render2D* render2D;
};