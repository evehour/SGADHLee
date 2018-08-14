#include "stdafx.h"
#include "ExportModel.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameModel.h"
#include "../Objects/GameTank.h"

ExportModel::ExportModel(ExecuteValues * values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	/*
	Export List::
	1. Material			-> .material (XML)
	2. Mesh + Bone		-> .mesh (binary)
	3. Animation		-> .animation (binary)

	*/

	////Tank
	//exporter = new Fbx::Exporter(Assets + L"Tank/Tank.fbx");
	//exporter->ExportMaterial(Models + L"/Tank/", L"Tank.material");
	//exporter->ExportMesh(Models + L"/Tank/", L"Tank.mesh");
	//SAFE_DELETE(exporter);

	////Cube
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.FBX");
	//exporter->ExportMaterial(Models + L"/Cube/", L"Cube.material");
	//exporter->ExportMesh(Models + L"/Cube/", L"Cube.mesh");
	//SAFE_DELETE(exporter);

	
	
	tank = new GameTank(Models + L"/Tank/", L"Tank.material");
	
	/*
	tank = new GameModel
	(
		Models + L"/Cube/", L"Cube.material",
		Models + L"/Cube/", L"Cube.mesh"
	);*/
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, Math::ToRadian(90));

	tank->RootAxis(R);

	int a = 10;
}

ExportModel::~ExportModel()
{
	SAFE_DELETE(tank);
}

void ExportModel::Update()
{
	tank->Update();
}

void ExportModel::PreRender()
{
	
}

void ExportModel::Render()
{
	tank->Render();
}

void ExportModel::PostRender()
{
	
}

void ExportModel::ResizeScreen()
{
}

