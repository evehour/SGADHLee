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

	//Test
	{
		testImage = new Texture(Textures + L"Moon.png");

		testViewRender2D = new Render2D();
		testRender2D = new Render2D(Shaders + L"990_Render2D.fx", 2049, 2049);
		testImageRender2D = new Render2D();
		testRenderTargetView = new RenderTargetView(2049, 2049);


		testRender2D->SRV(terrain->GetHeightMapSRV());
		testRender2D->Scale(2049, 2049);

		testImageRender2D->UseCenterPosition(true);
		testImageRender2D->SRV(testImage->SRV());
		testImageRender2D->Scale(100, 100);

		testViewRender2D->Scale(600, 600);
		testViewRender2D->SRV(testRenderTargetView->SRV());
	}
}

TestTerrain::~TestTerrain()
{
	SAFE_DELETE(testRenderTargetView);
	SAFE_DELETE(testViewRender2D);
	SAFE_DELETE(testImageRender2D);
	SAFE_DELETE(testRender2D);
	SAFE_DELETE(testImage);

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
#ifdef __USING_PATHFINDING__
		if (Mouse::Get()->Down(0))
		{
			MapTile* mapTile = NULL;
			mapTile = pathFinding->GetTile(D3DXVECTOR2(pickWorldPosition.x, pickWorldPosition.z));
			if (mapTile != NULL)
				pickMapTileWorldPos = mapTile->WorldPosition;
		}
#endif
	}
	desc.material->GetShader()->AsScalar("IsHovering")->SetBool(picked);
	desc.material->GetShader()->AsVector("PickPosition")->SetFloatVector(D3DXVECTOR2(pickPixel.x, pickPixel.z));
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
	testRenderTargetView->Clear();
	D3D::Get()->SetRenderTarget(testRenderTargetView->RTV(), testRenderTargetView->DSV());
	testRenderTargetView->GetViewport()->RSSetViewport();
	testRender2D->Render();
	testImageRender2D->Render();

	D3D::Get()->SetRenderTarget();
	Context::Get()->GetViewport()->RSSetViewport();
	testViewRender2D->Render();
	D3D::Get()->SetRenderTarget();
	//-----------------------------------------------------------------------------
}

void TestTerrain::PostRender()
{
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
