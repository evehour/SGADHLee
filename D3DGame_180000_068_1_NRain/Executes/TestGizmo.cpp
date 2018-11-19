#include "stdafx.h"
#include "TestGizmo.h"

#include "./Renders/Gizmo/Gizmo.h"

TestGizmo::TestGizmo(ExecuteValues * values)
	:Execute(values)
{
	gizmo = new Gizmo(values);
}

TestGizmo::~TestGizmo()
{
	SAFE_DELETE(gizmo);
}

void TestGizmo::Update()
{
	if (Keyboard::Get()->Down('1'))
		gizmo->ActiveMode = Gizmo::Mode::Translate;
	if (Keyboard::Get()->Down('2'))
		gizmo->ActiveMode = Gizmo::Mode::Rotate;
	if (Keyboard::Get()->Down('3'))
		gizmo->ActiveMode = Gizmo::Mode::NonUniformScale;
	if (Keyboard::Get()->Down('4'))
		gizmo->ActiveMode = Gizmo::Mode::UniformScale;

	//gizmo->Update();
}

void TestGizmo::PreRender()
{
	
}

void TestGizmo::Render()
{
	gizmo->Render();
}

void TestGizmo::PostRender()
{
}

