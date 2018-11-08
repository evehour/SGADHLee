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
	plane->SetDiffuse(1, 1, 1);

	cube = new MeshCube();
	cube->Scale(3, 3, 3);
	cube->Position(-10, 1.5f, 0);
	cube->SetShader(shader);
	cube->SetDiffuse(0, 0.7f, 0);

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
	pointLightBuffer = new PointLightBuffer();
	spotLightBuffer = new SpotLightBuffer();
	capsuleLightBuffer = new CapsuleLightBuffer();
}

TestAmbient::~TestAmbient()
{
	SAFE_DELETE(capsuleLightBuffer);
	SAFE_DELETE(spotLightBuffer);
	SAFE_DELETE(pointLightBuffer);
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
	ImGui::Separator();
	ImGui::ColorEdit4("DirectionColor", (float *)&ambientBuffer->Data.Color);
	ImGui::ColorEdit3("AmbientFloor", (float *)&ambientBuffer->Data.Floor);
	ImGui::ColorEdit3("AmbientCeil", (float *)&ambientBuffer->Data.Ceil);

	ImGui::Separator();
	ImGui::SliderFloat("Exp", &specularBuffer->Data.Exp, 1, 100);
	ImGui::SliderFloat("Intensity", &specularBuffer->Data.Intensity, 0, 10);

	ImGui::Separator();
	ImGui::SliderFloat3("PointLight Pos", (float *)&pointLightBuffer->Data.Position, -50, 50);
	ImGui::SliderFloat("PointLight Range", &pointLightBuffer->Data.Range, 0, 100);
	ImGui::ColorEdit4("PointLight Color", (float *)&pointLightBuffer->Data.Color);

	ImGui::Separator();
	ImGui::ColorEdit4("Spot Color", (float *)&spotLightBuffer->Data.Color);
	ImGui::SliderFloat3("Spot Pos", (float *)&spotLightBuffer->Data.Position, -50, 50);
	ImGui::SliderFloat("Spot Range", &spotLightBuffer->Data.Range, 1, 100);
	ImGui::SliderFloat3("Spot Dir", (float *)&spotLightBuffer->Data.Direction, -1, 1);

	ImGui::SliderFloat("Spot Inner", &spotLightBuffer->Data.Inner, 0, 180);
	ImGui::SliderFloat("Spot Outer", &spotLightBuffer->Data.Outer, 0, 180);

	ImGui::Separator();
	ImGui::ColorEdit4("Capsule Color", (float *)&capsuleLightBuffer->Data.Color);
	ImGui::SliderFloat3("Capsule Pos", (float *)&capsuleLightBuffer->Data.Position, -50, 50);
	ImGui::SliderFloat("Capsule Range", &capsuleLightBuffer->Data.Range, 1, 100);
	ImGui::SliderFloat3("Capsule Dir", (float *)&capsuleLightBuffer->Data.Direction, -1, 1);
	ImGui::SliderFloat("Capsule Length", &capsuleLightBuffer->Data.Length, 0, 180);

	ambientBuffer->SetPSBuffer(10);
	specularBuffer->SetPSBuffer(11);
	pointLightBuffer->SetPSBuffer(12);
	spotLightBuffer->SetPSBuffer(13);
	capsuleLightBuffer->SetPSBuffer(9);

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

