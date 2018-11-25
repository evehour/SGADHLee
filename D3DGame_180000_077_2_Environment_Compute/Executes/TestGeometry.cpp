#include "stdafx.h"
#include "TestGeometry.h"

#include "./Environment/Sky.h"
#include "./Environment/Terrain.h"
#include "./Environment/Grass.h"
#include "./Environment/NRain.h"
#include "./Environment/Rain.h"

TestGeometry::TestGeometry()
{
	sky = new Sky();
	terrain = new Terrain();
	grass = new Grass(terrain);
#ifdef __USE_NOMAL_RAIN
	rain = new Rain(D3DXVECTOR3(256, 256, 256));
#else
	rain = new NRain();
#endif
}

TestGeometry::~TestGeometry()
{
	SAFE_DELETE(grass);
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
	SAFE_DELETE(rain);
}

void TestGeometry::Update()
{
	grass->Update();
	sky->Update();
	rain->Update();
}

void TestGeometry::PreRender()
{
	sky->PreRender();
}

void TestGeometry::Render()
{
	sky->Render();
	terrain->Render();
	rain->Render();
	grass->Render();
	
	sky->RayleighMieRender();
}

void TestGeometry::PostRender()
{

}
