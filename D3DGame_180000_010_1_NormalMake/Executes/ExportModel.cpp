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

	//Tank
	exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.FBX");
	exporter->ExportMaterial(Models + L"/Cube/", L"Cube.material");
	exporter->ExportMesh(Models + L"/Cube/", L"Cube.mesh");
	SAFE_DELETE(exporter);

	
	/*tank = new GameModel
	(
		Models + L"/Tank/", L"Tank.material",
		Models + L"/Tank/", L"Tank.mesh"
	);*/
	tank = new GameModel
	(
		Models + L"/Cube/", L"Cube.material",
		Models + L"/Cube/", L"Cube.mesh"
	);
	tank->SetBoundSpace(GameModel::BOUND_TYPE_BOX);

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

