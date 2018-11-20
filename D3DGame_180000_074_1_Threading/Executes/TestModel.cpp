#include "stdafx.h"
#include "TestModel.h"

#include "./Objects/GameModel.h"



TestModel::TestModel()
{

	shader1 = new Shader(Shaders + L"052_Model.fx");
	shader2 = new Shader(Shaders + L"052_Model2.fx");
#if 1
	modelCount = 100;

	myThread = new thread*[modelCount];

	models = new GameModel*[modelCount];
	for (UINT i = 0; i < modelCount; i++)
	{
		myThread[i] = NULL;

		models[i] = new GameModel(
			Models + L"Kachujin/", L"Kachujin.material",
			Models + L"Kachujin/", L"Kachujin.mesh"
			, 30.0f
		);
		if (i % 2 != 0)
		{
			models[i]->SetShader(shader1);
			models[i]->Scale(0.0625f, 0.0625f, 0.0625f);
		}
		else
		{
			models[i]->SetShader(shader2);
			models[i]->Scale(0.125f, 0.125f, 0.125f);
		}


		D3DXVECTOR3 pos = D3DXVECTOR3((float)i * 3.0f, 0, 0);
		models[i]->Position(pos);
	}
#else
	model = new GameModel(
		//Materials + L"Meshes/", L"Cube.material",
		//Models + L"Meshes/", L"Cube.mesh"
		Models + L"Kachujin/", L"Kachujin.material",
		Models + L"Kachujin/", L"Kachujin.mesh"
	);

	model->SetShader(shader);
#endif

	freeVar = 0.0f;
}

TestModel::~TestModel()
{
	SAFE_DELETE(shader1);
	SAFE_DELETE(shader2);
#if 1
	for (UINT i = 0; i < modelCount; i++)
	{
		SAFE_DELETE(models[i]);
	}
	SAFE_DELETE_ARRAY(models);
	SAFE_DELETE_ARRAY(myThread);
#else
	SAFE_DELETE(model);
#endif

}

void TestModel::Update()
{
	shader1->AsScalar("freeVar")->SetFloat(freeVar);
	freeVar += 1.0f * Time::Delta();

#if 1
	for (UINT i = 0; i < modelCount; i++)
	{
		models[i]->Update();
	}
#else
	for (UINT i = 0; i < modelCount; i++)
	{
		myThread[i] = models[i]->UpdateThread();
	}


	for (UINT i = 0; i < modelCount; i++)
	{
		if (myThread[i] && myThread[i]->joinable())
			myThread[i]->join();
	}
#endif
}

void TestModel::PreRender()
{

}

void TestModel::Render()
{
	for (UINT i = 0; i < modelCount; i++)
		models[i]->Render();
}

void TestModel::PostRender()
{

}


