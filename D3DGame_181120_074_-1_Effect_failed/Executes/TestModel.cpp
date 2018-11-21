#include "stdafx.h"
#include "TestModel.h"

#include "./Objects/GameModel.h"



TestModel::TestModel()
{
	shader = new Shader(Shaders + L"052_Model2.fx");

	modelCount = 100;

	bunny = new GameModel(
		shader,
		Models + L"Kachujin/", L"Kachujin.material",
		Models + L"Kachujin/", L"Kachujin.mesh"
	);

	Context::GetGlobalLight()->Color = D3DXCOLOR(1, 0, 0, 1);

	bunny->Scale(0.01f, 0.01f, 0.01f);
	//bunny->SetDiffuseMap(Textures + L"Red.png");
}

TestModel::~TestModel()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(bunny);

}

void TestModel::Update()
{
	bunny->Update();
}

void TestModel::PreRender()
{

}

void TestModel::Render()
{
	bunny->Render();
}

void TestModel::PostRender()
{

}


