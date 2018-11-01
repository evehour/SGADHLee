#include "stdafx.h"
#include "TestLandscape.h"
#include "../Landscape/Sky.h"
#include "../Landscape/TerrainRender.h"
#include "../Landscape/Water.h"

TestLandscape::TestLandscape(ExecuteValues * values)
	: Execute(values)
{
	sky = new Sky(values);
	terrain = new TerrainRender();
	water = new Water(values, 64, 64, 4, terrain->HeightMap());
}

TestLandscape::~TestLandscape()
{
	SAFE_DELETE(water);
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
}

void TestLandscape::Update()
{
	sky->Update();
	terrain->Update();
	water->Update();
}

void TestLandscape::PreRender()
{

}

void TestLandscape::Render()
{
	sky->Render();
	terrain->Render();
	water->Render();
}

void TestLandscape::PostRender()
{
	
}

void TestLandscape::ResizeScreen()
{
}

