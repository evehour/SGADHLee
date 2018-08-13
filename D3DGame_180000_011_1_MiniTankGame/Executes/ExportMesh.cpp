#include "stdafx.h"
#include "ExportMesh.h"
#include "../Fbx/Exporter.h"

#include "Objects/GameModel.h"

ExportMesh::ExportMesh(ExecuteValues * values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	////Capsule
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Capsule.FBX");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Capsule.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Capsule.mesh");
	//SAFE_DELETE(exporter);

	////Cube
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.FBX");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Cube.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Cube.mesh");
	//SAFE_DELETE(exporter);

	////Cylinder
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cylinder.FBX");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Cylinder.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Cylinder.mesh");
	//SAFE_DELETE(exporter);

	////Plane
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Plane.FBX");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Plane.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Plane.mesh");
	//SAFE_DELETE(exporter);

	////Quad
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Quad.FBX");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Quad.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Quad.mesh");
	//SAFE_DELETE(exporter);

	////Sphere
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Sphere.FBX");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Sphere.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Sphere.mesh");
	//SAFE_DELETE(exporter);

	////Teapot
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Teapot.FBX");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Teapot.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Teapot.mesh");
	//SAFE_DELETE(exporter);

	for (int i = 0; i < 1; i++)
	{
		GameModel* model = new GameModel
		(
			Materials + L"/Meshes/", L"Teapot.material",
			Models + L"/Meshes/", L"Teapot.mesh"
		);

		D3DXVECTOR3 temp;
		temp.x = Math::Random(-20.0f, 20.0f);
		temp.y = 0;
		temp.z = Math::Random(-20.0f, 20.0f);
		model->Position(temp);

		D3DXMATRIX R;
		D3DXMatrixRotationY(&R, Math::ToRadian(Math::Random(-90.0f, 90.0f)));

		model->RootAxis(R);
		models.push_back(model);
	}

}

ExportMesh::~ExportMesh()
{
	for (GameModel* model : models)
		SAFE_DELETE(model);
}

void ExportMesh::Update()
{
	for (GameModel* model : models)
		model->Update();

}

void ExportMesh::PreRender()
{
	
}

void ExportMesh::Render()
{
	for (GameModel* model : models)
		model->Render();
}

void ExportMesh::PostRender()
{
	
}

void ExportMesh::ResizeScreen()
{

}

