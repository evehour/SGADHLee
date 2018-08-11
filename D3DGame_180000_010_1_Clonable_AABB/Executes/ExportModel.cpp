#include "stdafx.h"
#include "ExportModel.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameModel.h"

ExportModel::ExportModel(ExecuteValues * values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	////Cube
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.fbx");
	//exporter->ExportMaterial(Models + L"/Meshes/Cube/", L"Cube.material");
	//SAFE_DELETE(exporter);

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

	////Sphere
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Sphere.FBX");
	//exporter->ExportMaterial(Models + L"/Sphere/", L"Sphere.material");
	//exporter->ExportMesh(Models + L"/Sphere/", L"Sphere.mesh");
	//SAFE_DELETE(exporter);

	
	tank = new GameModel
	(
		Models + L"/Tank/", L"Tank.material",
		Models + L"/Tank/", L"Tank.mesh"
	);

	/*tank = new GameModel
	(
		Models + L"/Cube/", L"Cube.material",
		Models + L"/Cube/", L"Cube.mesh"
	);*/


	tank2 = new GameModel
	(
		Models + L"/Cube/", L"Cube.material",
		Models + L"/Cube/", L"Cube.mesh"
	);

	tank2->Position(D3DXVECTOR3(2, 1, 0));
}

ExportModel::~ExportModel()
{
	SAFE_DELETE(tank);
	SAFE_DELETE(tank2);
}

void ExportModel::Update()
{
	tank->Update();
	tank2->Update();
}

void ExportModel::PreRender()
{
	
}

void ExportModel::Render()
{
	tank->Render();
	tank2->Render();
}

void ExportModel::PostRender()
{
	
}

void ExportModel::ResizeScreen()
{
}

