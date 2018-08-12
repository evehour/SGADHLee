#include "stdafx.h"
#include "ExportModel.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameModel.h"

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

	//Cube
	exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.FBX");
	exporter->ExportMaterial(Models + L"/Cube/", L"Cube.material");
	exporter->ExportMesh(Models + L"/Cube/", L"Cube.mesh");
	SAFE_DELETE(exporter);

	
	/*
	gameModel = new GameModel
	(
		Models + L"/Tank/", L"Tank.material",
		Models + L"/Tank/", L"Tank.mesh"
	);
	*/

	gameModel = new GameModel
	(
		Models + L"/Cube/", L"Cube.material",
		Models + L"/Cube/", L"Cube.mesh"
	);

	int a = 10;
}

ExportModel::~ExportModel()
{
	SAFE_DELETE(gameModel);
}

void ExportModel::Update()
{
	gameModel->Update();
}

void ExportModel::PreRender()
{
	
}

void ExportModel::Render()
{
	gameModel->Render();
}

void ExportModel::PostRender()
{
	
}

void ExportModel::ResizeScreen()
{
}

