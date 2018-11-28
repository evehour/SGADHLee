#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Perspective.h"
#include "./Viewer/Viewport.h"

#include "./Viewer/Freedom.h"
#include "./Viewer/OrbitCamera.h"

#include "./Executes/Export.h"
#include "./Executes/TestEffect.h"
#include "./Executes/TestLine.h"
#include "./Executes/TestModel.h"
#include "./Executes/TestAnimModel.h"
#include "./Executes/TestGeometry.h"
#include "./Executes/TestInstancing.h"
#include "./Executes/TestStreamOutput.h"
#include "./Executes/TestCompute.h"

#include "./Model/Model.h"

Program::Program()
{
	Context::Create();
#if 1
	Context::Get()->GetMainCamera()->Position(0, 0, -5);
#else
	Context::Get()->GetMainCamera()->RotationDegree(20, 6);
	Context::Get()->GetMainCamera()->Position(200, 42, -27);
	((Freedom*)Context::Get()->GetMainCamera())->MoveSpeed(100.0f);
	//((Freedom*)Context::Get()->GetMainCamera())->RotationSpeed(20.0f);
#endif
	executes.push_back(new Export());
	//executes.push_back(new TestGeometry());
	//executes.push_back(new TestInstancing());
	//executes.push_back(new TestStreamOutput());
	executes.push_back(new TestCompute());
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	Context::Delete();
	Models::Delete();
	Shaders::Delete();
}

void Program::Update()
{
	Context::Get()->Update();

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	//Context::Get()->ViewProjectionSetting();

	for (Execute* exe : executes)
		exe->PreRender();
}

void Program::Render()
{
	Context::Get()->GetViewport()->RSSetViewport();
	ImGui::Begin("Environment");
	ImGui::SliderFloat3("Direction", (float*)&Context::Get()->GetGlobalLight()->Direction, -1, 1);
	ImGui::Separator();
	ImGui::End();
	Context::Get()->ChangeGlobalLight();

	for (Execute* exe : executes)
		exe->Render();

	//ImGui::Begin("Debug");
	//{
	//	ImGui::Text("FPS: %.2f", Time::Get()->FPS());
	//}
	//ImGui::End();
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
	Context::Get()->GetMainCamera()->Position(&vec);
	str = String::Format(L"CameraPos : %.2f, %.2f, %.2f", vec.x, vec.y, vec.z);
	DirectWrite::RenderText(str, rect, 12);

	rect.top += 12;
	D3DXVECTOR2 rot;
	Context::Get()->GetMainCamera()->RotationDegree(&rot);
	str = String::Format(L"CameraRot : %.0f, %.0f", rot.x, rot.y);
	DirectWrite::RenderText(str, rect, 12);
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	Context::Get()->GetPerspective()->Set(desc.Width, desc.Height);
	Context::Get()->GetViewport()->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();
}