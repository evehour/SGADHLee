#include "stdafx.h"
#include "DrawLandscape.h"
#include "../Landscape/Sky.h"
#include "../Landscape/Terrain.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/Billboard.h"

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

	//Create MeshSpheres
	{
		specular = new Shader(Shaders + L"025_Specular.hlsl");
		for (UINT i = 0; i < 10; i++)
		{

			diffuseColor = D3DXCOLOR(1, 0, 0, 1);
			specularColor = D3DXCOLOR(1, 1, 1, 1);
			shininess = 15.0f;

			MeshSphere* sphere = new MeshSphere();
			sphere->SetShader(specular);
			sphere->SetDiffuseMap(Textures + L"Wall.png");
			sphere->SetSpecularMap(Textures + L"Wall_specular.png");

			D3DXMATRIX S;
			D3DXMatrixScaling(&S, 5, 5, 5);
			sphere->RootAxis(S);

			D3DXVECTOR3 P;
			P.x = Math::Random(0.0f, 255.0f);
			P.z = Math::Random(0.0f, 255.0f);
			P.y = terrain->Y(P);
			P.y += 5.0f * 0.5f;

			sphere->Position(P);

			spheres.push_back(sphere);
		}
	}

	//Create Trees
	{
		for (int i = 0; i < 0; i++)
		{
			Billboard* tree = new Billboard(values, Shaders + L"024_Billboard.hlsl", Textures + L"Tree.png");
			D3DXVECTOR3 P;
			P.x = Math::Random(0.0f, 255.0f);
			P.z = Math::Random(0.0f, 255.0f);
			P.y = terrain->Y(P);

			D3DXVECTOR3 S;
			S.x = Math::Random(10.0f, 30.0f);
			S.y = Math::Random(10.0f, 30.0f);
			S.z = 1;

			bool fixedY = Math::Random(0, 1) == 0 ? false : true;
			tree->FixedY(fixedY);

			P.y += S.y * 0.5f;

			tree->Position(P);
			tree->Scale(S);
			
			trees.push_back(tree);
		}
	}
}

DrawLandscape::~DrawLandscape()
{
	for (Billboard* tree : trees)
		SAFE_DELETE(tree);

	for (MeshSphere* sphere : spheres)
		SAFE_DELETE(sphere);

	SAFE_DELETE(sky);
	SAFE_DELETE(terrain);
}

void DrawLandscape::Update()
{
	//D3DXVECTOR3 position = sphere->Position();
	//
	//if (Keyboard::Get()->Press('I'))
	//	position.z += 10.0f * Time::Delta();
	//else if (Keyboard::Get()->Press('K'))
	//	position.z -= 10.0f * Time::Delta();

	//if (Keyboard::Get()->Press('J'))
	//	position.x -= 10.0f * Time::Delta();
	//else if (Keyboard::Get()->Press('L'))
	//	position.x += 10.0f * Time::Delta();

	////position.y = terrain->Y(position);
	//D3DXVECTOR3 newPosition;
	//if (terrain->Y(&newPosition, position) == true)
	//	position.y = newPosition.y;

	//sphere->Position(position);
	
	// Picking
	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);

	D3DXVECTOR3 direction = values->MainCamera->Direction(
		values->Viewport, values->Perspective
	);
	D3DXVECTOR3 pickedPos;

	sky->Update();
	terrain->Update();

	isPicked = false;
	for (MeshSphere* sphere : spheres)
	{
		if (!isPicked && sphere->IsPick(start, direction, pickedPos))
		{
			isPicked = true;
		}
		sphere->Update();
	}

	for (Billboard* tree : trees)
		tree->Update();
}

void DrawLandscape::PreRender()
{

}

void DrawLandscape::Render()
{
	sky->Render(); // 젤먼저 그려야 함.

	terrain->Render();

	for (MeshSphere* sphere : spheres)
		sphere->Render();

	for (Billboard* tree : trees)
		tree->Render();
}

void DrawLandscape::PostRender()
{
	ImGui::Separator();

	ImGui::SliderFloat3("Diffuse", (float *)&diffuseColor, 0, 1);
	ImGui::SliderFloat3("Specular", (float *)&specularColor, 0, 1);
	ImGui::SliderFloat("Shininess", &shininess, 0.1f, 30.0f);

	ImGui::Separator();


	sphere->SetDiffuse(diffuseColor);
	sphere->SetSpecular(specularColor);
	sphere->SetShininess(shininess);

	if (terrain->GetBrushBuffer()->Data.Type > 0)
	{
		D3DXVECTOR3 picked;
		terrain->Y(&picked);

		ImGui::LabelText("Picked", "%.2f, %.2f, %.2f", picked.x, picked.y, picked.z);
	}
}

void DrawLandscape::ResizeScreen()
{
}

