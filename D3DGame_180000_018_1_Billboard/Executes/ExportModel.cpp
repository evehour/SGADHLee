#include "stdafx.h"
#include "ExportModel.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameModel.h"
#include "../Objects/GameTank.h"

ExportModel::ExportModel(ExecuteValues * values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;

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

