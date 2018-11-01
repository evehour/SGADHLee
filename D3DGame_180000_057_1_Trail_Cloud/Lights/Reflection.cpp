#include "stdafx.h"
#include "Reflection.h"

#include "../Viewer/Fixity.h"
#include "../Viewer/Perspective.h"
#include "../Objects/GameModel.h"

Reflection::Reflection(ExecuteValues * values)
	:values(values)
{
	shader = new Shader(Shaders + L"042_Reflection.hlsl");

	fixity = new Fixity();
	fixity->Position(0, 10, 0);
	fixity->RotationDegree(90, 0);

	refectionTarget = new RenderTarget(values, 2048, 2048);

	// Perspective
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		// pi * 0.5f(90도)를 주면 화면비율을 그대로 사용한다는 의미(정투영)
		perspective = new Perspective(1, 1, Math::PI * 0.5f, 1, 100);
	}

	vpBuffer = new ViewProjectionBuffer();
}

Reflection::~Reflection()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(refectionTarget);

	SAFE_DELETE(fixity);
	SAFE_DELETE(perspective);

	SAFE_DELETE(vpBuffer);
}

void Reflection::Add(GameModel * obj)
{
	objs.push_back(obj);
}

void Reflection::Update()
{
	
}

void Reflection::PreRender()
{
	refectionTarget->Set();

	D3DXMATRIX V, P;
	fixity->Matrix(&V);
	values->ViewProjection->SetView(V);

	perspective->GetMatrix(&P);
	values->ViewProjection->SetProjection(P);

	values->ViewProjection->SetVSBuffer(0);

	//for (GameModel* obj : objs)
	//{
	//	obj->Update();
	//	obj->Render();
	//}
	objs[0]->Update();
	objs[0]->Render();
	objs[1]->Update();
}

void Reflection::Render()
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
		ID3D11ShaderResourceView* srv;
		srv = refectionTarget->GetSRV();
		D3D::GetDC()->PSSetShaderResources(5, 1, &srv);
		obj->SetShader(shader);
		obj->Render();
	}

}
