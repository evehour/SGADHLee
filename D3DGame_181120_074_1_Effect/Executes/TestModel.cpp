#include "stdafx.h"
#include "TestModel.h"

TestModel::TestModel()
{
	shader = new Shader(Shaders + L"052_Model.fx");

	model = new GameModel(
		shader,
		Models + L"Kachujin/", L"Kachujin.material",
		Models + L"Kachujin/", L"Kachujin.mesh"
	);
	model->Scale(0.0125f, 0.0125f, 0.0125f);

	Context::GetGlobalLight()->Color = D3DXCOLOR(1, 0, 0, 0);
}

TestModel::~TestModel()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(model);
}

void TestModel::Update()
{
	model->Update();
}

void TestModel::PreRender()
{

}

void TestModel::Render()
{
	model->Render();
}

void TestModel::PostRender()
{

}


