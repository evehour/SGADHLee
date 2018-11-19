#include "stdafx.h"
#include "TestGizmo.h"

#include "./Renders/Gizmo/GizmoComponent.h"

#include "./Draw/MeshCube.h"

TestGizmo::TestGizmo(ExecuteValues * values)
	:Execute(values)
{
	gizmo = new GizmoComponent(values);

	cubeMaterial = new Material(Shaders + L"014_Model_3.hlsl");
	cubeMaterial->SetDiffuseMap(Textures + L"Dirt2.png");
	cube[0] = new MeshCube(cubeMaterial, 1, 1, 1);
	cube[1] = new MeshCube(cubeMaterial, 1, 1, 1);
	cube[1]->Position(0, 0, 5);

	gizmo->AddEntity(cube[0]);
	gizmo->AddEntity(cube[1]);
}

TestGizmo::~TestGizmo()
{
	SAFE_DELETE(cubeMaterial);

	for (UINT i = 0; i < 2; i++)
		SAFE_DELETE(cube[i]);

	SAFE_DELETE(gizmo);
}

void TestGizmo::Update()
{
	for (UINT i = 0; i < 2; i++)
		cube[i]->Update();
	gizmo->Update();
}

void TestGizmo::PreRender()
{
	
}

void TestGizmo::Render()
{
	for (UINT i = 0; i < 2; i++)
		cube[i]->Render();
	gizmo->Render();
}

void TestGizmo::PostRender()
{
}

