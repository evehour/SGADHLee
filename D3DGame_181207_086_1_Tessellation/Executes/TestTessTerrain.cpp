#include "stdafx.h"
#include "TestTessTerrain.h"
#include "../Environment/Terrain.h"

TestTessTerrain::TestTessTerrain()
{
	material = new Material(Shaders + L"060_TessTerrain.fx");
	material->SetDiffuseMap(Textures + L"Dirt.png");
	wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";

	terrain = new Terrain(material, heightMap);
}

TestTessTerrain::~TestTessTerrain()
{
	SAFE_DELETE(material);
	SAFE_DELETE(terrain);
}

void TestTessTerrain::Update()
{
	terrain->Update();
}

void TestTessTerrain::PreRender()
{

}

void TestTessTerrain::Render()
{
	terrain->Render();
}

void TestTessTerrain::PostRender()
{

}
