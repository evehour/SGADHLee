#include "stdafx.h"
#include "DrawLandscape.h"
#include "../Landscape/Sky.h"
#include "../Landscape/Terrain.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshCube.h"
#include "../Objects/Billboard.h"
#include "../Objects/PointLight.h"
#include "../Objects/SpotLight.h"

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

	
	//Create PointLight
	{
		pointLighting = new Shader(Shaders + L"027_PointLight.hlsl");
		pointLight = new PointLight();

		//PointLight::Desc desc;
		//desc.Position = D3DXVECTOR3(85, 10, 30);
		//desc.Color = D3DXVECTOR3(1, 0, 0);
		//desc.Intensity = 2;
		//desc.Range = 5;
		//pointLight->Add(desc);

		//desc.Position = D3DXVECTOR3(95, 10, 30);
		//desc.Color = D3DXVECTOR3(0, 0, 1);
		//desc.Intensity = 2;
		//desc.Range = 5;
		//pointLight->Add(desc);
	}

	//Create SpotLight
	{
		spotLighting = new Shader(Shaders + L"028_SpotLight.hlsl");
		spotLight = new SpotLight();

		SpotLight::Desc desc;
		desc.Position = D3DXVECTOR3(85, 5, 30);
		desc.Color = D3DXVECTOR3(0, 1, 0);
		desc.Direction = D3DXVECTOR3(0, -1, 0);
		desc.InnerAngle = 0.25f;
		desc.OuterAngle = 0.75f;
		spotLight->Add(desc);
	}


	//Create Specular
	{
		specular = new Shader(Shaders + L"025_Specular.hlsl");

		diffuseColor = D3DXCOLOR(1, 0, 0, 1);
		specularColor = D3DXCOLOR(1, 1, 1, 1);
		shininess = 15.0f;

		sphere = new MeshSphere();
		//sphere->SetShader(specular);
		//sphere->SetShader(pointLighting);
		sphere->SetShader(spotLighting);
		sphere->SetDiffuseMap(Textures + L"Wall.png");
		sphere->SetSpecularMap(Textures + L"Wall_specular.png");

		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 5, 5, 5);
		sphere->RootAxis(S);

		sphere->Position(D3DXVECTOR3(80, 10, 30));
	}

	//Create NormalMap
	{
		normalMap = new Shader(Shaders  + L"026_NormalMap.hlsl");

		diffuseColor = D3DXCOLOR(1, 0, 0, 1);
		specularColor = D3DXCOLOR(1, 1, 1, 1);
		shininess = 15.0f;

		cube = new MeshCube();
		//cube->SetShader(normalMap);
		//cube->SetShader(pointLighting);
		cube->SetShader(spotLighting);
		cube->SetDiffuseMap(Textures + L"Wall.png");
		cube->SetSpecularMap(Textures + L"Wall_specular.png");
		cube->SetNormalMap(Textures + L"Wall_normal.png");

		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 5, 5, 5);
		cube->RootAxis(S);

		cube->Position(D3DXVECTOR3(90, 2.5f, 30));

		normalBuffer = new NormalBuffer();
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

	SAFE_DELETE(pointLight);
	SAFE_DELETE(spotLight);

	SAFE_DELETE(normalBuffer);
	SAFE_DELETE(cube)
	SAFE_DELETE(sphere);
	SAFE_DELETE(sky);
	SAFE_DELETE(terrain);
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

	pointLight->Update();
	spotLight->Update();

	sky->Update();
	terrain->Update();
	sphere->Update();
	cube->Update();

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
	sphere->Render();

	normalBuffer->SetPSBuffer(10);
	cube->Render();

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

	ImGui::Separator();

	ImGui::SliderInt("Visible Normal", &normalBuffer->Data.VisibleNormal, 0, 1);

	ImGui::Separator();

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

