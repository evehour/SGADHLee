#include "stdafx.h"
#include "Program.h"

#include "./Executes/ExportModel.h"
#include "./Executes/ExportMesh.h"
#include "./Executes/DrawLandscape.h"

Program::Program()
{
	RenderSettings::Create();

	
	RenderSettings::MainCamera()->RotationDegree(24, 0);
	RenderSettings::MainCamera()->Position(82.18f, 27.00f, -29.00f);

	executes.push_back(new ExportModel());
	executes.push_back(new ExportMesh());
	executes.push_back(new DrawLandscape());
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	RenderSettings::Delete();
}

void Program::Update()
{
	RenderSettings::MainCamera()->Update();

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{

}

void Program::Render()
{
	RenderSettings::RenderLight();
	RenderSettings::RenderMainViewProjection();


	ImGui::Text("FPS : %f", Time::Get()->FPS());
	ImGui::Separator();
	ImGui::SliderFloat3("Direction", (float *)&RenderSettings::Light()->Data.Direction, -1, 1);
	ImGui::Separator();

	D3DXVECTOR3 vec;
	RenderSettings::MainCamera()->Position(&vec);
	ImGui::LabelText("CameraPos", "%.2f, %.2f, %.2f", vec.x, vec.y, vec.z);

	D3DXVECTOR2 rot;
	RenderSettings::MainCamera()->RotationDegree(&rot);
	ImGui::LabelText("CameraRot", "%.2f, %.2f", rot.x, rot.y);

	for (Execute* exe : executes)
		exe->Render();
}

void Program::Resize()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	RenderSettings::MainProjection()->Set(desc.Width, desc.Height);
	RenderSettings::MainViewport()->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->Resize();
}