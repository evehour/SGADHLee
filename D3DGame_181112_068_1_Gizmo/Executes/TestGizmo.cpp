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
	cube = new MeshCube(cubeMaterial, 1, 1, 1);
	cube->Position(0.0f, 0.0f, -0.7f);

	gizmo->AddEntity(cube);
}

TestGizmo::~TestGizmo()
{
	SAFE_DELETE(cubeMaterial);
	SAFE_DELETE(cube);
	SAFE_DELETE(gizmo);
}

void TestGizmo::Update()
{
	cube->Update();
	gizmo->Update();
}

void TestGizmo::PreRender()
{
	
}

void TestGizmo::Render()
{
	cube->Render();
	gizmo->Render();
}

void TestGizmo::PostRender()
{
}

