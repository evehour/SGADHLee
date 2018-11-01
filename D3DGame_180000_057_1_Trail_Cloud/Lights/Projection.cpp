#include "stdafx.h"
#include "Projection.h"

#include "../Viewer/Fixity.h"
#include "../Viewer/Perspective.h"
#include "../Objects/GameModel.h"

Projection::Projection(ExecuteValues * values)
	:values(values)
{
	shader = new Shader(Shaders + L"042_Projection.hlsl");

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

Projection::~Projection()
{
	SAFE_DELETE(shader);
	
	SAFE_DELETE(fixity);
	SAFE_DELETE(perspective);

	SAFE_DELETE(vpBuffer);
}

void Projection::Add(GameModel * obj)
{
	objs.push_back(obj);
}

void Projection::Update()
{
	for (GameModel* obj : objs)
	{
		obj->SetShader(shader);
		obj->SetNormalMap(Textures + L"Bricks.png");
		//obj->Update();
	}

	D3DXMATRIX V, P;
	fixity->Matrix(&V);
	vpBuffer->SetView(V);

	perspective->GetMatrix(&P);
	vpBuffer->SetProjection(P);
}

void Projection::PreRender()
{
}

void Projection::Render()
{
	D3DXVECTOR3 pos;
	fixity->Position(&pos);
	ImGui::DragFloat3("Proj Position", (float*)pos, 0.5f);
	fixity->Position(pos.x, pos.y, pos.z);

	D3DXVECTOR2 rot;
	fixity->RotationDegree(&rot);
	ImGui::DragFloat2("Proj Rotation", (float*)rot, 0.5f);
	fixity->RotationDegree(rot.x, rot.y);

	vpBuffer->SetVSBuffer(10);

	for (GameModel* obj : objs)
	{
		obj->Render();
	}
}
