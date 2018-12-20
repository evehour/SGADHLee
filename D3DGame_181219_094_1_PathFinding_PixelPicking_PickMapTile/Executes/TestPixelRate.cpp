#include "stdafx.h"
#include "TestPixelRate.h"

TestPixelRate::TestPixelRate()
{
	image = new Texture(Textures + L"lenna.png");
	render2D = new Render2D();
	renderTargetView = new RenderTargetView(2049, 2049);
}

TestPixelRate::~TestPixelRate()
{
	SAFE_DELETE(image);
}

void TestPixelRate::Update()
{

}

void TestPixelRate::PreRender()
{

}

void TestPixelRate::Render()
{
}

void TestPixelRate::PostRender()
{

}
