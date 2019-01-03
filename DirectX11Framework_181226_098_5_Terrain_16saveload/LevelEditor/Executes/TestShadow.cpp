#include "stdafx.h"
#include "TestShadow.h"

void TestShadow::Initialize()
{
	Loading->AdjPercent(100);
}

void TestShadow::Ready()
{
	meshMaterial = new Material(Shaders + L"055_MeshShadow.fx");
	meshMaterial->SetDiffuse(1, 1, 1, 1);

	quad = new MeshQuad(meshMaterial, 200, 200);
	quad->RotationDegree(90, 0, 0);
	sphere = new MeshSphere(meshMaterial, 5);
	sphere->Position(0, 5.0f, 0);
	sun = new MeshSphere(meshMaterial, 5);

	render2D = new Render2D(0, 2048, 2048);
	render2D->UseCenterPosition(false);
	render2D->Scale(600, 600);
	render2D->SRV(Shadowmap->SRV());
}

void TestShadow::Destroy()
{
	SAFE_DELETE(sun);
	SAFE_DELETE(sphere);
	SAFE_DELETE(quad);

	SAFE_DELETE(meshMaterial);
	SAFE_DELETE(render2D);
}

void TestShadow::Update()
{
	sun->Position(Context::Get()->GetGlobalLight()->Position);
}

void TestShadow::PreRender()
{
	Shadowmap->Set(true);
	quad->Render(0, 2);
	sphere->Render(0, 2);
}

void TestShadow::Render()
{
	meshMaterial->GetShader()->AsShaderResource("DepthMap")->SetResource(Shadowmap->DSRV());
	quad->Render(0, 3);
	sphere->Render(0, 3);
	meshMaterial->GetShader()->AsShaderResource("DepthMap")->SetResource(0);
	meshMaterial->GetShader()->Apply(0, 3);
	sun->Render();
	render2D->Render();
}

void TestShadow::PostRender()
{
}

void TestShadow::ResizeScreen()
{
}
