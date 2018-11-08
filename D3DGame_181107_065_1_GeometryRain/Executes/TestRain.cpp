#include "stdafx.h"
#include "TestRain.h"
#include "../Objects/MeshPlane.h"
#include "../Landscape/Rain.h"


TestRain::TestRain(ExecuteValues * values)
	:Execute(values)
{
	planeShader = new Shader(Shaders + L"042_Plane.hlsl");
	plane = new MeshPlane();
	plane->Scale(10, 1, 10);
	plane->SetDiffuse(0, 0, 1, 1);
	plane->SetShader(planeShader);

	rain = new Rain(values, D3DXVECTOR3(100, 100, 100), 500);
}

TestRain::~TestRain()
{

	SAFE_DELETE(planeShader);
	SAFE_DELETE(plane);
	SAFE_DELETE(rain);
}

void TestRain::Update()
{
	rain->Update();
	plane->Update();
}

void TestRain::PreRender()
{

}

void TestRain::Render()
{
	plane->Render();
	rain->Render();
}

void TestRain::PostRender()
{

}


