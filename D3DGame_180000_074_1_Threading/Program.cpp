#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Perspective.h"
#include "./Viewer/Viewport.h"

#include "./Viewer/Freedom.h"
#include "./Viewer/OrbitCamera.h"

#include "./Executes/TestEffect.h"
#include "./Executes/TestLine.h"
#include "./Executes/TestModel.h"

#include "./Model/Model.h"

Program::Program()
{
	Context::Create();
	Context::GetMainCamera()->Position(0, 0, -5);

	//executes.push_back(new TestLine());
	executes.push_back(new TestModel());
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	Context::Delete();
	Models::Delete();
}

void Program::Update()
{
	Context::GetMainCamera()->Update(); // 카메라 업데이트

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
	for (Execute* exe : executes)
		exe->Render();

	ImGui::Begin("Debug");
	{
		ImGui::Text("FPS: %.2f", Time::Get()->FPS());
	}
	ImGui::End();
}

void Program::PostRender()
{
	for (Execute* exe : executes)
		exe->PostRender();

	//wstring str;
	//RECT rect = { 0, 0, 300, 300 };

	//str = String::Format(L"FPS : %.0f", ImGui::GetIO().Framerate);
	//DirectWrite::RenderText(str, rect, 12);

	//rect.top += 12;
	//D3DXVECTOR3 vec;
	//Context::GetMainCamera()->Position(&vec);
	//str = String::Format(L"CameraPos : %.2f, %.2f, %.2f", vec.x, vec.y, vec.z);
	//DirectWrite::RenderText(str, rect, 12);

	//rect.top += 12;
	//D3DXVECTOR2 rot;
	//Context::GetMainCamera()->RotationDegree(&rot);
	//str = String::Format(L"CameraRot : %.0f, %.0f", rot.x, rot.y);
	//DirectWrite::RenderText(str, rect, 12);
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	Context::GetPerspective()->Set(desc.Width, desc.Height);
	Context::GetViewport()->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();
}