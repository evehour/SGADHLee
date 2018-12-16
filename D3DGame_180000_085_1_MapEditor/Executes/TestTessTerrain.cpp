#include "stdafx.h"
#include "TestTessTerrain.h"
#include "../Environment/Terrain.h"

#include "../Objects/GameModel.h"

TestTessTerrain::TestTessTerrain()
{
	material = new Material(Shaders + L"060_TessTerrain2.fx");
	material->SetDiffuseMap(Textures + L"Dirt.png");
	wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";

	terrain = new Terrain(material, heightMap);

	gameModel = new GameModel(Shaders + L"052_Model.fx",
		Materials + L"Meshes/", L"Quad.material",
		Models + L"Meshes/", L"Quad.mesh"
	);
}

TestTessTerrain::~TestTessTerrain()
{
	SAFE_DELETE(gameModel);

	SAFE_DELETE(material);
	SAFE_DELETE(terrain);
}

void TestTessTerrain::Update()
{
	terrain->Update();

	gameModel->Update();
}

void TestTessTerrain::PreRender()
{

}

void TestTessTerrain::Render()
{
	terrain->Render();

	gameModel->Render();
}

void TestTessTerrain::PostRender()
{
	D3DXVECTOR3 gmPosition;
	gmPosition = gameModel->Position();

	if(ImGui::DragFloat3("GM::Position", (float*)&gmPosition))
	{
		gmPosition.y = terrain->GetHeight(gmPosition.x, gmPosition.z);
		gameModel->Position(gmPosition);
	}
}
