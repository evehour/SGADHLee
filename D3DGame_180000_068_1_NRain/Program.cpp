#include "stdafx.h"
#include "Program.h"
#include "./Viewer/Freedom.h"
#include "./Viewer/OrbitCamera.h"

#include "./Executes/Export.h"
#include "./Executes/TestRain.h"
#include "./Executes/TestInstancing.h"
#include "./Executes/TestStreamOutput.h"
#include "./Executes/TestGizmo.h"

Program::Program()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	//values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Perspective = new Perspective(desc.Width, desc.Height, (float)(D3DX_PI * 0.25), 0.1f, 1000.0f);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	//values->MainCamera = new OrbitCamera(100);
	values->MainCamera = new Freedom(10);

	values->MainCamera->Position(0, 0, -5);
	values->GlobalLight->Data.Direction = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	values->GlobalLight->Data.Position = D3DXVECTOR3(0.0f, -10.0f, 0.0f);

	//SamplerState Fix
	{
		ID3D11SamplerState* state = nullptr;
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;


		HRESULT hr;

		hr = D3D::GetDevice()->CreateSamplerState(&desc, &state);
		assert(SUCCEEDED(hr));

		for (UINT i = 0;i < 16;i++)
		{
			D3D::GetDC()->PSSetSamplers(i, 1, &state);
		}

		SAFE_RELEASE(state);
	}

	baseSamplerState = new SamplerState();
	baseSamplerState->PSSetSamplers(0);
	baseSamplerState->PSSetSamplers(1);
	baseSamplerState->PSSetSamplers(2);
	baseSamplerState->PSSetSamplers(3);
	
	//baseSamplerState->PSSetSamplers(4);

	executes.push_back(new Export(values));

	//executes.push_back(new TestInstancing(values));
	executes.push_back(new TestRain(values));
	//executes.push_back(new TestExecute(values));
	//executes.push_back(new TestGizmo(values));
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(values->MainCamera);
	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values);

	SAFE_DELETE(baseSamplerState);
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
	ImGui::SliderFloat3("Direction", (float *)&values->GlobalLight->Data.Direction, -1, 1);
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
	str = String::Format(L"CameraPos : %.2f, %.2f, %.2f", vec.x, vec.y, vec.z);
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