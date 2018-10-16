#include "stdafx.h"
#include "Shadow.h"

#include "../Viewer/Perspective.h"
#include "../Objects/GameModel.h"

Shadow::Shadow(ExecuteValues * values)
	:values(values)
{
	values->GlobalLight->Data.Position = D3DXVECTOR3(0, 10, 20);
	shaderDepth = new Shader(Shaders + L"043_Shadow.hlsl", "VS_Depth", "PS_Depth");
	depthShadow = new RenderTarget(values, 2048, 2048);

	shaderRender = new Shader(Shaders + L"043_Shadow.hlsl", "VS_Shadow", "PS_Shadow");

	depthRender = new Render2D(values);
	depthRender->Position(0, 0);
	depthRender->Scale(400, 400);

	// Perspective
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		// width height 너비 높이에 대한 비율임
		// pi * 0.5f 로 준다는 화면각 그대로 쓴다는거 1,1 비율까지 이렇게하면 화면 그대로 쓴다는거
		// 정투영
		perspective = new Perspective(1, 1, Math::PI * 0.5f, 1, 1000);
	}

	vpBuffer = new ViewProjectionBuffer();

	samplerState[0] = new SamplerState();
	samplerState[1] = new SamplerState();
	samplerState[1]->ComparisonFunc(D3D11_COMPARISON_LESS_EQUAL);
	samplerState[1]->AddressU(D3D11_TEXTURE_ADDRESS_MIRROR);
	samplerState[1]->AddressV(D3D11_TEXTURE_ADDRESS_MIRROR);
	samplerState[1]->Filter(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);

	buffer = new Buffer();
}

Shadow::~Shadow()
{
	SAFE_DELETE(buffer);

	for (UINT i = 0; i < 2; i++)
		SAFE_DELETE(samplerState[i]);

	SAFE_DELETE(shaderRender);

	SAFE_DELETE(depthRender);
	SAFE_DELETE(depthShadow);
	SAFE_DELETE(shaderDepth);

	SAFE_DELETE(perspective);

	SAFE_DELETE(vpBuffer);
}

void Shadow::Add(GameModel * obj)
{
	objs.push_back(obj);
}

void Shadow::Update()
{
	depthRender->Update();
}

void Shadow::PreRender()
{
	depthShadow->Set();

	// 나중에 fixity가 조명 방향으로 바꿔야함
	D3DXMATRIX V, P;
	D3DXMatrixLookAtLH(
		&V,
		&values->GlobalLight->Data.Position,
		&D3DXVECTOR3(0, 0, 0),
		&D3DXVECTOR3(0, 1, 0)
	);
	values->ViewProjection->SetView(V);

	perspective->GetMatrix(&P);
	values->ViewProjection->SetProjection(P);
	values->ViewProjection->SetVSBuffer(0);

	vpBuffer->SetView(V);
	vpBuffer->SetProjection(P);

	for (GameModel* obj : objs)
	{
		obj->SetShader(shaderDepth);

		obj->Update();
		obj->Render();
	}
}

void Shadow::Render()
{
	ImGui::SliderFloat3("Light Position",
		(float*)&values->GlobalLight->Data.Position, -100, 100);
	D3DXVec3Normalize(&values->GlobalLight->Data.Direction, &values->GlobalLight->Data.Position);
	values->GlobalLight->Data.Direction = -values->GlobalLight->Data.Direction;

	ImGui::DragFloat("Shadow Bias", &buffer->Data.Bias, 0.00001f, 0.0f, 1.0f, "%.5f");

	vpBuffer->SetVSBuffer(10);
	ID3D11ShaderResourceView* view = depthShadow->GetSRV();
	D3D::GetDC()->PSSetShaderResources(10, 1, &view);

	buffer->SetPSBuffer(10);
	samplerState[1]->PSSetSamplers(10);
	for (GameModel* obj : objs)
	{
		obj->SetShader(shaderRender);

		obj->Render();
	}
	samplerState[1]->PSSetSamplers(0);

	depthRender->SRV(depthShadow->GetSRV());
	depthRender->Update();
	depthRender->Render();
}