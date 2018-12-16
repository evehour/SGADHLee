#include "stdafx.h"
#include "DrawLandscape.h"
#include "../Landscape/Terrain.h"
#include "../Objects/MeshSphere.h"

DrawLandscape::DrawLandscape(ExecuteValues * values)
	: Execute(values)
{
	//Create Terrain
	{
		wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
		Material* material = new Material(Shaders + L"020_HW_TerrainBrush+Splatting.hlsl");
		//Material* material = new Material(Shaders + L"019_TerrainBrush.hlsl");
		
		terrain = new Terrain(values, material);
	}
}

DrawLandscape::~DrawLandscape()
{
	SAFE_DELETE(terrain);
}

void DrawLandscape::Update()
{
	D3DXVECTOR3 position;// = sphere->Position();
	
	if (Keyboard::Get()->Press('I'))
		position.z += 10.0f * Time::Delta();
	else if (Keyboard::Get()->Press('K'))
		position.z -= 10.0f * Time::Delta();

	if (Keyboard::Get()->Press('J'))
		position.x -= 10.0f * Time::Delta();
	else if (Keyboard::Get()->Press('L'))
		position.x += 10.0f * Time::Delta();

	//position.y = terrain->Y(position);
	//D3DXVECTOR3 newPosition;
	//if (terrain->Y(&newPosition, position) == true)
	//	position.y = newPosition.y;
	
	terrain->Update();
}

void DrawLandscape::PreRender()
{

}

void DrawLandscape::Render()
{
	terrain->Render();
}

void DrawLandscape::PostRender()
{
	//D3DXVECTOR3 picked;
	//terrain->Y(&picked);

	//ImGui::LabelText("Picked", "%.2f, %.2f, %.2f", picked.x, picked.y, picked.z);
}

void DrawLandscape::ResizeScreen()
{
}

