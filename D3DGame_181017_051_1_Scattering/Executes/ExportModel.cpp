#include "stdafx.h"
#include "ExportModel.h"
#include "../Fbx/Exporter.h"

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
}

ExportModel::~ExportModel()
{
	
}

void ExportModel::Update()
{
	
}

void ExportModel::PreRender()
{
	
}

void ExportModel::Render()
{
	
}

void ExportModel::PostRender()
{
	
}

void ExportModel::ResizeScreen()
{
}

