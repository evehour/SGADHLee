#include "stdafx.h"
#include "TestTerrain.h"

#include "Environment/CubeSky.h"
#include "Environment/Terrain.h"

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
	}
}

TestTerrain::~TestTerrain()
{
	SAFE_DELETE(sky);
	SAFE_DELETE(terrain);
}

void TestTerrain::Update()
{
	terrain->Update();
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
	D3DXVECTOR3 position;
	bool picked = false;
	picked = terrain->Picking(position);

	ImGui::Begin("Environment");
	{
		ImGui::Separator();
		ImGui::Text("Pick: %s", (picked ? "True" : "False"));
		Terrain::InitDesc desc = terrain->Desc();
		ImGui::Text("Brush: %f, %f", position.x, position.y);
		ImGui::Text("Mouse: %f, %f", Mouse::Get()->GetPosition().x, Mouse::Get()->GetPosition().y);
		ImGui::Separator();
	}
	ImGui::End();
}
