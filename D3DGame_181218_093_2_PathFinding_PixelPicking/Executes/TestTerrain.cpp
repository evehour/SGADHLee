#include "stdafx.h"
#include "TestTerrain.h"

#include "Environment/CubeSky.h"
#include "Environment/Terrain.h"
#include "Environment/PathFinding.h"

TestTerrain::TestTerrain()
{
	sky = new CubeSky(Textures + L"sunsetcube1024.dds");

	// Create Terrain
	{
		Terrain::InitDesc desc =
		{
			Textures + L"Terrain/terrain.raw",
			50.0f,
			2049, 2049,
			{
				//Textures + L"Terrain/grass.png",
				Textures + L"Dirt3.png",
				Textures + L"Terrain/darkdirt.png",
				Textures + L"Terrain/stone.png",
				Textures + L"Terrain/lightdirt.png",
				Textures + L"Terrain/snow.png",
			},
			Textures + L"Terrain/blend.png",

			0.5f,
		};

		desc.material = new Material(Shaders + L"060_Terrain_ImagePicking.fx");
		terrain = new Terrain(desc);
		pathFinding = NULL;
		//pathFinding = new PathFinding(terrain, 2);
	}
}

TestTerrain::~TestTerrain()
{
	SAFE_DELETE(sky);
	SAFE_DELETE(terrain);
	SAFE_DELETE(pathFinding);
}

void TestTerrain::Update()
{

}

void TestTerrain::PreRender()
{

}

void TestTerrain::Render()
{
	sky->Render();
	terrain->Render();
}

void TestTerrain::PostRender()
{

}
