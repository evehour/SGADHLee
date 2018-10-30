#include "stdafx.h"
#include "TestAmbient.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshCube.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshBunny.h"

TestAmbient::TestAmbient(ExecuteValues * values)
	: Execute(values)
{
	shader = new Shader(Shaders + L"045_Lighting.hlsl");

	plane = new MeshPlane();
	plane->Scale(10, 1, 10);
	plane->SetShader(shader);
	plane->SetDiffuse(1, 0, 0);

	cube = new MeshCube();
	cube->Scale(3, 3, 3);
	cube->Position(-10, 1.5f, 0);
	cube->SetShader(shader);
	cube->SetDiffuse(0, 0.7, 0);

	sphere = new MeshSphere();
	sphere->Scale(3, 3, 3);
	sphere->Position(10, 1.5f, 0);
	sphere->SetShader(shader);
	sphere->SetDiffuse(0, 0, 1);

	sphere2 = new MeshSphere();
	sphere2->Scale(3, 3, 3);
	sphere2->Position(10, 1.5f, 10);
	sphere2->SetShader(shader);
	sphere2->SetDiffuse(1, 1, 1);

	bunny = new MeshBunny();
	bunny->Scale(0.02f, 0.02f, 0.02f);
	bunny->Position(0, 5.0f, 0);
	bunny->SetShader(shader);
	bunny->SetDiffuse(1, 1, 1);
	bunny->SetDiffuseMap(Textures + L"White.png");

	ambientBuffer = new AmbientBuffer();
	specularBuffer = new SpecularBuffer();
}

TestAmbient::~TestAmbient()
{
	SAFE_DELETE(specularBuffer);
	SAFE_DELETE(ambientBuffer);
	SAFE_DELETE(bunny);
	SAFE_DELETE(sphere2);
	SAFE_DELETE(sphere);
	SAFE_DELETE(cube);
	SAFE_DELETE(plane);
	SAFE_DELETE(shader);
}

void TestAmbient::Update()
{
	sphere->Update();
	sphere2->Update();
	cube->Update();
	plane->Update();
	bunny->Update();
}

void TestAmbient::PreRender()
{

}

void TestAmbient::Render()
{
	ImGui::SliderFloat4("AmbientColor", (float *)&ambientBuffer->Data.Color, 0, 1);
	ImGui::SliderFloat3("AmbientFloor", (float *)&ambientBuffer->Data.Floor, 0, 1);
	ImGui::SliderFloat3("AmbientCeil", (float *)&ambientBuffer->Data.Ceil, 0, 1);

	ImGui::SliderFloat("Exp", &specularBuffer->Data.Exp, 1, 100);
	ImGui::SliderFloat("Intensity", &specularBuffer->Data.Intensity, 0, 10);

	ambientBuffer->SetPSBuffer(10);
	specularBuffer->SetPSBuffer(11);

	sphere->Render();
	sphere2->Render();
	cube->Render();
	plane->Render();
	bunny->Render();
}

void TestAmbient::PostRender()
{
	
}

void TestAmbient::ResizeScreen()
{

}

