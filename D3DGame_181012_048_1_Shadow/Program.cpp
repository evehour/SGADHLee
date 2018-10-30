#include "stdafx.h"
#include "Program.h"
#include "./Viewer/Freedom.h"
#include "./Executes/Export.h"
//#include "./Executes/TestDepth.h"
#include "./Executes/TestAnimation.h"
#include "./Executes/TestVersus.h"
#include "./Executes/TestLandscape.h"
#include "./Executes/TestBlend.h"
#include "./Executes/TestMultiTexture.h"
#include "./Executes/TestProjection.h"
#include "./Executes/TestShadow.h"
#include "./Executes/TestReflection.h"

Program::Program()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	values->MainCamera = new Freedom(100);
	//values->MainCamera->RotationDegree(24, 0);
	//values->MainCamera->Position(82.18f, 27.00f, -29.00f);
	
	//values->MainCamera->RotationDegree(30, 0);
	//values->MainCamera->Position(-2.0f, 10.0f, -20.00f);

	//values->MainCamera->RotationDegree(90, 0);
	//values->MainCamera->Position(0.0f, 100.0f, -550.00f);
	//values->MainCamera->Position(0.0f, 120.0f, -250.00f);
	//values->MainCamera->Position(0.0f, 0.0f, -10.00f);
	
	//values->MainCamera->RotationDegree(30, 0);
	//values->MainCamera->Position(230, 186, -136);
	
	values->MainCamera->Position(0, 0, -20);
	values->GlobalLight->Data.Direction = D3DXVECTOR3(-1, -1, 1);

	executes.push_back(new Export(values));

	//executes.push_back(new TestDepth(values));
	//executes.push_back(new TestAnimation(values));
	//executes.push_back(new TestVersus(values));
	//executes.push_back(new TestLandscape(values));
	//executes.push_back(new TestBlend(values));
	//executes.push_back(new TestMultiTexture(values));
	//executes.push_back(new TestProjection(values));
	executes.push_back(new TestShadow(values));
	//executes.push_back(new TestReflection(values));
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

	ImGui::Text("FPS : %.2f", Time::Get()->FPS());
//	ImGui::SliderFloat3("Direction", (float *)&values->GlobalLight->Data.Direction, -1, 1);
}

void Program::PostRender()
{
	for (Execute* exe : executes)
		exe->PostRender();

	wstring str;
	RECT rect = { 0, 0, 300, 300 };

	str = String::Format(L"FPS : %.0f", ImGui::GetIO().Framerate);
	DirectWrite::RenderText(str, rect, 12);

	rect.top += 12;
	D3DXVECTOR3 vec;
	values->MainCamera->Position(&vec);
	str = String::Format(L"CameraPos : %.0f, %.0f, %.0f", vec.x, vec.y, vec.z);
	DirectWrite::RenderText(str, rect, 12);

	rect.top += 12;
	D3DXVECTOR2 rot;
	values->MainCamera->RotationDegree(&rot);
	str = String::Format(L"CameraRot : %.0f, %.0f", rot.x, rot.y);
	DirectWrite::RenderText(str, rect, 12);
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