#include "stdafx.h"
#include "DrawLandscape.h"
#include "../Landscape/Sky.h"
#include "../Landscape/Terrain.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/Billboard.h"

DrawLandscape::DrawLandscape()
{
	sky = new Sky();	

	//Create Terrain
	{
		terrain = new Terrain();
		terrain->GetMaterial()->SetShader(Shaders + L"022_TerrainBrushLine.hlsl");
	}

	//Create MeshSphere
	{
		sphere = new MeshSphere();

		sphere->Scale(3, 3, 3);
		sphere->Position(D3DXVECTOR3(80, 10, 30));
	}

	//Create Trees
	{
		for (int i = 0; i < 1; i++)
		{
			Billboard* tree = new Billboard(Shaders + L"024_Billboard.hlsl", Textures + L"Tree.png");

			D3DXVECTOR3 P;
			P.x = Math::Random(0.0f, 255.0f);
			P.z = Math::Random(0.0f, 255.0f);
			P.y = terrain->Y(P);

			D3DXVECTOR3 S;
			S.x = Math::Random(10.0f, 30.0f);
			S.y = Math::Random(10.0f, 30.0f);
			S.z = 1;

			//P.y += S.y * 0.5f;

			tree->Position(P);
			tree->Scale(S);

			bool fixedY = Math::Random(0, 1) == 0 ? false : true;
			tree->FixedY(fixedY);

			trees.push_back(tree);
		}
	}
}

DrawLandscape::~DrawLandscape()
{
	for (Billboard* tree : trees)
		SAFE_DELETE(tree);

	SAFE_DELETE(sphere);
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
}

void DrawLandscape::Update()
{
	D3DXVECTOR3 position = sphere->Position();
	
	if (Keyboard::Get()->Press('I'))
		position.z += 10.0f * Time::Delta();
	else if (Keyboard::Get()->Press('K'))
		position.z -= 10.0f * Time::Delta();

	if (Keyboard::Get()->Press('J'))
		position.x -= 10.0f * Time::Delta();
	else if (Keyboard::Get()->Press('L'))
		position.x += 10.0f * Time::Delta();

	//position.y = terrain->Y(position);
	D3DXVECTOR3 newPosition;
	if (terrain->Y(&newPosition, position) == true)
		position.y = newPosition.y;

	sphere->Position(position);

	sky->Update();
	terrain->Update();
	sphere->Update();

	for (Billboard* tree : trees)
		tree->Update();
}

void DrawLandscape::PreRender()
{

}

void DrawLandscape::Render()
{
	sky->Render();
	terrain->Render();
	sphere->Render();

	for (Billboard* tree : trees)
		tree->Render();

	if (terrain->GetBrushBuffer()->Data.Type > 0)
	{
		D3DXVECTOR3 picked;
		terrain->Y(&picked);

		ImGui::LabelText("Picked", "%.2f, %.2f, %.2f", picked.x, picked.y, picked.z);
	}
}

void DrawLandscape::Resize()
{
}

