#include "stdafx.h"
#include "DrawLandscape.h"
#include "../Landscape/Terrain.h"
#include "../Objects/MeshSphere.h"

DrawLandscape::DrawLandscape(ExecuteValues * values)
	: Execute(values)
{
	// Create Terrain
	{
		wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
		Material* material = new Material(Shaders + L"017_Terrain.hlsl");
		material->SetDiffuseMap(Textures + L"Dirt2.png");

		terrain = new Terrain(material, heightMap, D3DXVECTOR3(3, 3, 3));
	}

	// Create MeshSphere
	{
		sphere = new MeshSphere();

		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 3, 3, 3);
		sphere->RootAxis(S);

		sphere->Position(D3DXVECTOR3(80, 10, 30));
	}
}

DrawLandscape::~DrawLandscape()
{
	SAFE_DELETE(terrain);
	SAFE_DELETE(sphere);
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

#if true
	//첫번째 방법
	position.y = terrain->Y(position);
#else
	//두번째 방법
	D3DXVECTOR3 newPosition;
	if (terrain->Y(&newPosition, position) == true)
		position.y = newPosition.y;
#endif
	sphere->Position(position);

	terrain->Update();
	sphere->Update();
}

void DrawLandscape::PreRender()
{

}

void DrawLandscape::Render()
{
	terrain->Render();
	sphere->Render();
}

void DrawLandscape::PostRender()
{

}

void DrawLandscape::ResizeScreen()
{
}
