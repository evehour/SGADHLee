#include "stdafx.h"
#include "TestGeometry.h"

#include "./Environment/Terrain.h"
#include "./Environment/Grass.h"
#include "./Environment/Rain.h"

TestGeometry::TestGeometry()
{
	terrain = new Terrain();
	grass = new Grass(terrain);
	rain = new Rain(D3DXVECTOR3(256, 256, 256));
}

TestGeometry::~TestGeometry()
{
	SAFE_DELETE(grass);
	SAFE_DELETE(terrain);
	SAFE_DELETE(rain);
}

void TestGeometry::Update()
{
	grass->Update();
	rain->Update();
}

void TestGeometry::PreRender()
{

}

void TestGeometry::Render()
{
	terrain->Render();
	rain->Render();
	grass->Render();
}

void TestGeometry::PostRender()
{

}
