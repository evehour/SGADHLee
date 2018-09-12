#include "stdafx.h"
#include "ExportAnim.h"
#include "../Fbx/Exporter.h"

#include "../Objects/GameModel.h"

ExportAnim::ExportAnim(ExecuteValues * values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	//Kachujin Mesh
	exporter = new Fbx::Exporter(Assets + L"Kachujin/Mesh.fbx");
	exporter->ExportMaterial(Models + L"Kachujin/", L"Kachujin.material");
	//exporter->ExportMesh(Models + L"Kachujin/", L"Kachujin.mesh", D3DXVECTOR3(0.20f, 0.20f, 0.20f));
	//exporter->ExportMesh(Models + L"Kachujin/", L"Kachujin.mesh", D3DXVECTOR3(1.50f, 1.50f, 1.50f));
	exporter->ExportMesh(Models + L"Kachujin/", L"Kachujin.mesh", D3DXVECTOR3(0.75f, 0.750f, 0.750f));
	//exporter->ExportMesh(Models + L"Kachujin/", L"Kachujin.mesh", D3DXVECTOR3(1.f, 1.f, 1.f));
	SAFE_DELETE(exporter);

	//exporter = new Fbx::Exporter(Assets + L"Kachujin/Samba_Dancing.fbx");
	//exporter->ExportAnimation(Models + L"Kachujin/", L"Samba_Dance.anim", 0);
	//SAFE_DELETE(exporter);
}

ExportAnim::~ExportAnim()
{
	
}

void ExportAnim::Update()
{
	
}

void ExportAnim::PreRender()
{
	
}

void ExportAnim::Render()
{
	
}

void ExportAnim::PostRender()
{
	
}

void ExportAnim::ResizeScreen()
{
}

