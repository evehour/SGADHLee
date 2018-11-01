#include "stdafx.h"
#include "TestSpace.h"
#include "../Objects/Planet.h"

TestSpace::TestSpace(ExecuteValues * values)
	: Execute(values)
{
	shader = new Shader(Shaders + L"028_Diffuse.hlsl");

	// Planets
	{
		float val;
		planetsCount = 1000;

		for (UINT i = 0; i < planetsCount; i++)
			planets[i] = NULL;

		for (UINT i = 0; i < 2; i++)
			planets[i] = new Planet();

		{
			val = 10.5f;
			planets[0]->SetInfo(Planet::PIF_ROTATION_SPEED, &val);
			planets[0]->SetInfo(Planet::PIF_ROTATION_AXIS, D3DXVECTOR3(Math::ToRadian(45), Math::ToRadian(20), 0));
			//planets[0]->Position(-2, 0, 0);
			//planets[0]->noRev = true;
		}

		{
			// 자전
			val = 0.5f;
			planets[1]->SetInfo(Planet::PIF_ROTATION_SPEED, &val);
			planets[1]->SetInfo(Planet::PIF_ROTATION_AXIS, D3DXVECTOR3(Math::ToRadian(45), Math::ToRadian(20), 0));

			// 공전
			val = 50.0f;
			planets[1]->SetInfo(Planet::PIF_REVOLUTION_SPEED, &val);
			planets[1]->SetInfo(Planet::PIF_REVOLUTION_AXIS, D3DXVECTOR3(0, Math::ToRadian(20), 0));
			val = 2.0f;
			planets[1]->SetInfo(Planet::PIF_REVOLUTION_DISTANCE, &val);
		}

		for (UINT i = 0; i < 2; i++)
		{
			planets[i]->SetShader(shader);
			planets[i]->SetDiffuseMap(Textures + L"Wall.png");
		}
	}

}

TestSpace::~TestSpace()
{
	SAFE_DELETE(shader);

	for (UINT i = 0; i < planetsCount; i++)
		SAFE_DELETE(planets[i]);
}

void TestSpace::Update()
{
	//planets[0]->GetTransform()->SetLocalRotation(a);
	for (UINT i = 0; i < 1; i++)
		planets[i]->Update();
}

void TestSpace::Render()
{
	for (UINT i = 0; i < 1; i++)
		planets[i]->Render();
}

void TestSpace::PostRender()
{
	//a = planets[0]->GetTransform()->GetAngle();
	ImGui::Begin("Space");
	{
		ImGui::SliderFloat3("Angle", (float *)&a, -360.0f, 360.0f);
	}
	ImGui::End();
}
