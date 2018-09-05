#include "stdafx.h"
#include "TestSpace.h"
#include "../Objects/Planet.h"

TestSpace::TestSpace(ExecuteValues * values)
	: Execute(values)
{
	planetsCount = 100;
	planets = new Planet[100];

	planets[0].SetInfo(Planet::PIF_ROTATION_SPEED, 20.0f);
	planets[0].Rotation(Math::ToRadian(45), Math::ToRadian(20), 0);

	shader = new Shader(Shaders + L"028_Diffuse.hlsl");
	planets[0].SetShader(shader);
	planets[0].SetDiffuseMap(Textures + L"Wall.png");
}

TestSpace::~TestSpace()
{
	SAFE_DELETE(shader);
	SAFE_DELETE_ARRAY(planets);
}

void TestSpace::Update()
{
	planets[0].Update();
}

void TestSpace::Render()
{
	planets[0].Render();
}
