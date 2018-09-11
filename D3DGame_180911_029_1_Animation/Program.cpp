#include "stdafx.h"
#include "Program.h"
#include "./Viewer/Freedom.h"
#include "./Executes/ExportModel.h"
#include "./Executes/ExportMesh.h"
#include "./Executes/ExportAnim.h"
//#include "./Executes/TestDepth.h"
#include "./Executes/TestAnimation.h"

Program::Program()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	values->MainCamera = new Freedom(8);
	//values->MainCamera->RotationDegree(24, 0);
	//values->MainCamera->Position(82.18f, 27.00f, -29.00f);
	values->MainCamera->RotationDegree(30, 0);
	values->MainCamera->Position(-2.0f, 10.0f, -20.00f);

	values->GlobalLight->Data.Direction = D3DXVECTOR3(-1, -1, 1);

	executes.push_back(new ExportModel(values));
	executes.push_back(new ExportMesh(values));
	executes.push_back(new ExportAnim(values));

	//executes.push_back(new TestDepth(values));
	executes.push_back(new TestAnimation(values));
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
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
	D3DXMATRIX view, projection;
	values->MainCamera->Matrix(&view);
	values->Perspective->GetMatrix(&projection);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

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
	values->Viewport->RSSetViewport();

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

	D3DXVECTOR3 vec;
	values->MainCamera->Position(&vec);
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