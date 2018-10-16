#include "stdafx.h"
#include "Shadow.h"

#include "../Viewer/Fixity.h"
#include "../Viewer/Perspective.h"
#include "../Objects/GameModel.h"

Shadow::Shadow(ExecuteValues * values)
	:values(values)
{
	shaderDepth = new Shader(Shaders + L"043_Shadow.hlsl", "VS_Depth", "PS_Depth");
	depthShadow = new RenderTarget(values, 2048, 2048);

	depthRender = new Render2D(values);
	depthRender->Position(0, 0);
	depthRender->Scale(200, 200);


	fixity = new Fixity();
	fixity->Position(0, 10, 0);
	fixity->RotationDegree(90, 0);

	// Perspective
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		// pi * 0.5f(90도)를 주면 화면비율을 그대로 사용한다는 의미(정투영)
		perspective = new Perspective(1, 1, Math::PI * 0.5f, 1, 100);
	}

	vpBuffer = new ViewProjectionBuffer();
}

Shadow::~Shadow()
{
	SAFE_DELETE(shaderDepth);

	SAFE_DELETE(fixity);
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

	D3DXMATRIX V, P;
	fixity->Matrix(&V);
	values->ViewProjection->SetView(V);

	perspective->GetMatrix(&P);
	values->ViewProjection->SetProjection(P);

	values->ViewProjection->SetVSBuffer(0);

	for (GameModel* obj : objs)
	{
		obj->SetShader(shaderDepth);

		obj->Update();
		obj->Render();
	}
}

void Shadow::Render()
{
	D3DXVECTOR3 pos;
	fixity->Position(&pos);
	ImGui::DragFloat3("Proj Position", (float*)pos, 0.5f);
	fixity->Position(pos.x, pos.y, pos.z);

	D3DXVECTOR2 rot;
	fixity->Rotation(&rot);
	ImGui::DragFloat2("Proj Rotation", (float*)rot, 0.5f);
	fixity->Rotation(rot.x, rot.y);

	for (GameModel* obj : objs)
	{
		obj->Render();
	}

	depthRender->SRV(depthShadow->GetSRV());
	depthRender->Update();
	depthRender->Render();
}
