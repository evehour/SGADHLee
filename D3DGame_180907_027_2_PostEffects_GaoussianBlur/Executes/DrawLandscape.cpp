#include "stdafx.h"
#include "DrawLandscape.h"
#include "../Landscape/Sky.h"
#include "../Landscape/Terrain.h"
#include "../Landscape/QuadTree.h"

DrawLandscape::DrawLandscape(ExecuteValues * values)
	: Execute(values)
{
	sky = new Sky(values);	

	//Create Terrain
	{
		wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
		Material* material = new Material(Shaders + L"022_TerrainBrushLine.hlsl");
		
		terrain = new Terrain(values, material);
	}

	quadTree = new QuadTree(values, terrain);
}

DrawLandscape::~DrawLandscape()
{
	SAFE_DELETE(quadTree);
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
}

void DrawLandscape::Update()
{
	sky->Update();
	//terrain->Update();

	quadTree->Update();
}

void DrawLandscape::PreRender()
{

}

void DrawLandscape::Render()
{
	sky->Render();
	//terrain->Render();

	quadTree->Render();
}

void DrawLandscape::PostRender()
{
	
}

void DrawLandscape::ResizeScreen()
{
}

