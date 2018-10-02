#include "stdafx.h"
#include "TestLandscape.h"
#include "../Landscape/Sky.h"
#include "../Landscape/Terrain.h"
#include "../Landscape/QuadTree.h"

TestLandscape::TestLandscape(ExecuteValues * values)
	: Execute(values)
{
	sky = new Sky(values);	

	//Create Terrain
	{		
		terrain = new Terrain(values);
	}

	quadTree = new QuadTree(values, terrain);
}

TestLandscape::~TestLandscape()
{
	SAFE_DELETE(quadTree);
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
}

void TestLandscape::Update()
{
	sky->Update();
	//terrain->Update();

	quadTree->Update();
}

void TestLandscape::PreRender()
{

}

void TestLandscape::Render()
{
	sky->Render();
	//terrain->Render();

	quadTree->Render();
}

void TestLandscape::PostRender()
{
	
}

void TestLandscape::ResizeScreen()
{
}

