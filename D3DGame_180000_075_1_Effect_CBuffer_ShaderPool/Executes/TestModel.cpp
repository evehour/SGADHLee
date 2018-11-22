#include "stdafx.h"
#include "TestModel.h"
#include "./Components/Gizmo/GizmoComponent.h"

TestModel::TestModel()
{
	gizmo = new GizmoComponent();

	modelCount = 100;

	models = new GameModel*[modelCount];
	for (UINT i = 0; i < modelCount; ++i)
	{
		models[i] = new GameModel(
			Shaders + L"052_Model.fx",
			Models + L"Kachujin/", L"Kachujin.material",
			Models + L"Kachujin/", L"Kachujin.mesh"
		);

		models[i]->SetBounder(GameModel::BOUND_TYPE_BOX);
		gizmo->AddEntity(models[i]);

		models[i]->Scale(0.0125f, 0.0125f, 0.0125f);
		models[i]->Position((float)i * 5.0f, 0, 0);
	}

	//Context::GetGlobalLight()->Color = D3DXCOLOR(1, 0, 0, 0);
}

TestModel::~TestModel()
{
	for (UINT i = 0;i < modelCount; i++)
		SAFE_DELETE(models[i]);
	SAFE_DELETE_ARRAY(models);

	SAFE_DELETE(gizmo);
}

void TestModel::Update()
{
	for (UINT i = 0;i < modelCount; i++)
		models[i]->Update();

	gizmo->Update();
}

void TestModel::PreRender()
{

}

void TestModel::Render()
{
	for (UINT i = 0;i < modelCount; i++)
		models[i]->Render();

	gizmo->Render();
}

void TestModel::PostRender()
{

}


