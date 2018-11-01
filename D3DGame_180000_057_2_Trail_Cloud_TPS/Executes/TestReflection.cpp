#include "stdafx.h"
#include "TestReflection.h"

#include "../Objects/GameAnimModel.h"
#include "../Objects/MeshPlane.h"
#include "../Model/ModelClip.h"
#include "../Lights/Reflection.h"

TestReflection::TestReflection(ExecuteValues * values)
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
		// �÷����� �ȳ����� ������ ī�޶� ���̰� ���� 0�̶� �׷��Ϲ�.
		plane->Scale(10, 1, 10);
		plane->SetDiffuse(1, 1, 1, 1);
	}

	reflection = new Reflection(values);
	reflection->Add(hanjo);
	reflection->Add(plane);
}

TestReflection::~TestReflection()
{
	SAFE_DELETE(hanjo);
	SAFE_DELETE(plane);
	SAFE_DELETE(modelShader);
	SAFE_DELETE(objectShader);
	SAFE_DELETE(reflection);
}

void TestReflection::Update()
{
	reflection->Update();

	//hanjo->Update();
	//plane->Update();
}

void TestReflection::PreRender()
{
	reflection->PreRender();
}

void TestReflection::Render()
{

	reflection->Render();
}

void TestReflection::PostRender()
{

}

void TestReflection::ResizeScreen()
{

}

