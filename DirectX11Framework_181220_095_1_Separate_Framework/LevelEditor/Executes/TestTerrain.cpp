#include "stdafx.h"
#include "TestTerrain.h"

#include "Environment/CubeSky.h"
#include "Environment/Terrain.h"
#include "Environment/TerrainBrush.h"
#include "Environment/PathFinding.h"
#include "Environment/HeightMap.h"

void TestTerrain::Initialize()
{
	sky = new CubeSky(Textures + L"sunsetcube1024.dds");
	Terrain::InitDesc desc;
	// Create Terrain
	{
		desc =
		{
			Textures + L"Terrain/terrain.raw",
			50.0f,
			2049, 2049,
			{
				Textures + L"Terrain/grass.png",
				//Textures + L"Dirt3.png",
				Textures + L"Terrain/darkdirt.png",
				Textures + L"Terrain/stone.png",
				Textures + L"Terrain/lightdirt.png",
				Textures + L"Terrain/snow.png",
			},
			Textures + L"Terrain/blend.png",

			0.5f,
		};

#ifdef __USE_BLENDMAP__
		desc.material = new Material(Shaders + L"060_Terrain.fx");
#else
		desc.material = new Material(Shaders + L"060_Terrain_InBlending.fx");
#endif

		terrain = new Terrain(desc);
#ifdef __USING_PATHFINDING__
		pathFinding = new PathFinding(terrain);
#endif
	}

	brush = new TerrainBrush(terrain, desc.material->GetShader(), desc.HeightMapWidth, desc.HeightMapHeight);
}

void TestTerrain::Ready()
{
}

void TestTerrain::Destroy()
{
	SAFE_DELETE(brush);

#ifdef __USING_PATHFINDING__
	SAFE_DELETE(pathFinding);
#endif
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
}

void TestTerrain::Update()
{
	terrain->Update();
	Terrain::InitDesc desc = terrain->Desc();

	picked = false;
	picked = terrain->Picking(&pickPixel, &pickWorldPosition);
	if (picked)
	{
		if (Mouse::Get()->Press(0))
		//if (Mouse::Get()->Down(0))
		{
#ifdef __USING_PATHFINDING__
			MapTile* mapTile = NULL;
			mapTile = pathFinding->GetTile(D3DXVECTOR2(pickWorldPosition.x, pickWorldPosition.z));
			if (mapTile != NULL)
				pickMapTileWorldPos = mapTile->WorldPosition;
#endif
			brush->Brush(true, 0.05f);
		}
		brush->Hovering(picked);
		brush->UpdatePosition(pickPixel);
	}
}

void TestTerrain::PreRender()
{
}

void TestTerrain::Render()
{
	sky->Render();
	terrain->Render();

	//-----------------------------------------------------------------------------
	// Test
	//brush->TestRender();
	//-----------------------------------------------------------------------------
	
	ImGui::Begin("Environment");
	{
		ImGui::Separator();
		ImGui::Text("Pick: %s", (picked ? "True" : "False"));
		Terrain::InitDesc desc = terrain->Desc();
		ImGui::Text("Brush: %f, %f", pickPixel.x, pickPixel.z);
		ImGui::Text("PickWPos: %f, %f", pickWorldPosition.x, pickWorldPosition.z);
		if (picked)
		{
#ifdef __USING_PATHFINDING__
			ImGui::Text("PickMTWPos: %f, %f", pickMapTileWorldPos.x, pickMapTileWorldPos.z);
#endif
		}
		ImGui::Text("Mouse: %f, %f", Mouse::Get()->GetPosition().x, Mouse::Get()->GetPosition().y);
		ImGui::Separator();
	}
	ImGui::End();
}

void TestTerrain::PostRender()
{
}

void TestTerrain::ResizeScreen()
{
}
