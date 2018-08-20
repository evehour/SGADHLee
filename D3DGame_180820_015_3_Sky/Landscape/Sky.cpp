#include "stdafx.h"
#include "Sky.h"
#include "../Objects/GameModel.h"

Sky::Sky(ExecuteValues* values)
	:values(values)
{
	model = new Model;
	model->ReadMaterial(Materials + L"Meshes/", L"Sphere.material");
	model->ReadMesh(Models + L"Meshes/", L"Sphere.mesh");

	for (Material* material : model->Materials())
	{
		material->SetShader(Shaders + L"021_Sky.hlsl");
	}

	buffer = new Buffer();

	//Create Rasterizer
	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		States::CreateRasterizer(&desc, &cullMode[0]);

		desc.FrontCounterClockwise = true;
		States::CreateRasterizer(&desc, &cullMode[1]);
	}
}

Sky::~Sky()
{
	SAFE_DELETE(buffer);
}

void Sky::Update()
{
	/*D3DXVECTOR3 camPos;
	values->MainCamera->Position(&camPos);

	model->Position(camPos);
	model->Update();*/
}

void Sky::Render()
{
	D3DXVECTOR3 camPos;
	values->MainCamera->Position(&camPos);

	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);
	W._41 = camPos.x;
	W._42 = camPos.y;
	W._43 = camPos.z + 0.5f;
	model->MeshByIndex(0)->SetWorld(W);

	//D3D::GetDC()->RSSetState(cullMode[1]);
	model->MeshByIndex(0)->Render();
	//D3D::GetDC()->RSSetState(cullMode[0]);
}
