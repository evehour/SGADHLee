#include "stdafx.h"
#include "TestProjection.h"

#include "../Objects/GameAnimModel.h"
#include "../Objects/MeshPlane.h"
#include "../Model/ModelClip.h"
#include "../Lights/Projection.h"

TestProjection::TestProjection(ExecuteValues * values)
	: Execute(values)
{
	modelShader = new Shader(Shaders + L"035_Animation.hlsl");
	objectShader = new Shader(Shaders + L"042_Plane.hlsl");
	
	// Model
	{
		hanjo = new GameAnimModel(
			Models + L"Kachujin/", L"Kachujin.material",
			Models + L"Kachujin/", L"Kachujin.mesh"
		);

		hanjo->SetShader(modelShader);
		hanjo->Scale(0.01f, 0.01f, 0.01f);

		hanjo->AddClip(Models + L"Kachujin/Running.anim");
		hanjo->Play((UINT)0, true, 0.0f, 10.0f);
	}

	// Mesh
	{
		plane = new MeshPlane();
		plane->SetShader(objectShader);
		// 플레인이 안나오는 이유는 카메라 높이가 지금 0이라서 그런겅미.
		plane->Scale(10, 1, 10);
		plane->SetDiffuse(1, 1, 1, 1);
	}

	projection = new Projection(values);
	projection->Add(hanjo);
	projection->Add(plane);
}

TestProjection::~TestProjection()
{
	SAFE_DELETE(hanjo);
	SAFE_DELETE(plane);
	SAFE_DELETE(modelShader);
	SAFE_DELETE(objectShader);
	SAFE_DELETE(projection);
}

void TestProjection::Update()
{
	projection->Update();

	hanjo->Update(true);
	plane->Update();
}

void TestProjection::PreRender()
{

}

void TestProjection::Render()
{
	hanjo->Render();
	plane->Render();

	projection->Render();
}

void TestProjection::PostRender()
{
	
}

void TestProjection::ResizeScreen()
{

}

