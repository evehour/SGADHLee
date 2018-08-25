#include "stdafx.h"
#include "Program.h"
#include "./Viewer/Freedom.h"
#include "./Viewer/Orbit.h"

#include "./Executes/ExportModel.h"
#include "./Executes/ExportMesh.h"

#include "./Landscape/Terrain.h"

#include "./Executes/DrawLandscape.h"
#include "./Executes/ExeTankGame.h"

Program::Program()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	{
		//values->MainCamera = new Freedom();
		//values->MainCamera->Position(82.18f, 27.00, -29.00f);
		//values->MainCamera->RotationDegree(25, 0);

		values->MainCamera_Option1 = new Freedom();
		values->MainCamera_Option1->Position(82.18f, 27.00, -29.00f);
		values->MainCamera_Option1->RotationDegree(25, 0);

		values->MainCamera_Option2 = new Orbit();
		values->MainCamera_Option2->RotationDegree(50, 0);

		values->MainCamera = values->MainCamera_Option1;

		cameraMode = 0;
	}

	wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
	Material* material = new Material(Shaders + L"022_TerrainBrushLine.hlsl");
	terrain = new Terrain(values, material);

	executes.push_back(new ExportModel(values));
	executes.push_back(new ExportMesh(values));
	executes.push_back(new DrawLandscape(values, terrain));
	executes.push_back(new ExeTankGame(values, terrain));
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(terrain);

	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values->MainCamera_Option1);
	SAFE_DELETE(values->MainCamera_Option2);
	SAFE_DELETE(values);
}

void Program::Update()
{
	values->MainCamera->Update();

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	for (Execute* exe : executes)
		exe->PreRender();
}

void Program::Render()
{
	D3DXMATRIX view, projection;
	values->MainCamera->Matrix(&view);
	values->Perspective->GetMatrix(&projection);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	values->GlobalLight->SetPSBuffer(0);

	for (Execute* exe : executes)
		exe->Render();
}

void Program::PostRender()
{
	for (Execute* exe : executes)
		exe->PostRender();

	ImGui::Text("FPS : %f", Time::Get()->FPS());
	ImGui::Separator();
	ImGui::SliderFloat3("Direction", (float *)&values->GlobalLight->Data.Direction, -1, 1);
	ImGui::Separator();

	if (ImGui::RadioButton("Freedom Camera", &cameraMode, 0))
	{
		values->MainCamera = values->MainCamera_Option1;
	}
	else if (ImGui::RadioButton("Orbit Camera", &cameraMode, 1))
	{
		values->MainCamera = values->MainCamera_Option2;
	}

	D3DXVECTOR3 vec;
	values->MainCamera->Position(&vec);
	//ImGui::SliderFloat3("CameraPosition", (float*)&vec, -300, 300);
	ImGui::LabelText("CameraPos", "%.2f, %.2f, %.2f", vec.x, vec.y, vec.z);

	D3DXVECTOR2 rot;
	values->MainCamera->RotationDegree(&rot);
	ImGui::LabelText("CameraRot", "%.2f, %.2f", rot.x, rot.y);
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values->Perspective->Set(desc.Width, desc.Height);
	values->Viewport->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();
}