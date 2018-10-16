#include "stdafx.h"
#include "TestShadow.h"

#include "../Objects/GameAnimModel.h"
#include "../Objects/MeshPlane.h"
#include "../Model/ModelClip.h"
#include "../Lights/Shadow.h"

TestShadow::TestShadow(ExecuteValues * values)
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
		hanjo->SetDiffuse(1, 1, 1, 1);
		hanjo->Scale(0.025f, 0.025f, 0.025f);

		hanjo->AddClip(Models + L"Kachujin/Samba_Dance.anim");
		hanjo->Play((UINT)0, true, 0.0f, 10.0f);
	}

	// Mesh
	{
		plane = new MeshPlane();
		plane->SetShader(objectShader);
		plane->SetDiffuseMap(Textures + L"White.png");
		// 플레인이 안나오는 이유는 카메라 높이가 지금 0이라서 그런겅미.
		plane->Scale(10, 1, 10);
		plane->SetDiffuse(1, 1, 1, 1);
	}

	shadow = new Shadow(values);
	shadow->Add(hanjo);
	shadow->Add(plane);
}

TestShadow::~TestShadow()
{
	SAFE_DELETE(hanjo);
	SAFE_DELETE(plane);
	SAFE_DELETE(modelShader);
	SAFE_DELETE(objectShader);
	SAFE_DELETE(shadow);
}

void TestShadow::Update()
{
	shadow->Update();
}

void TestShadow::PreRender()
{
	shadow->PreRender();
}

void TestShadow::Render()
{

	shadow->Render();
}

void TestShadow::PostRender()
{
	
}

void TestShadow::ResizeScreen()
{

}

