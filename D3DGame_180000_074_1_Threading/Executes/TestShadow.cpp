#include "stdafx.h"
#include "TestShadow.h"

#include "./Objects/GameModel.h"



TestShadow::TestShadow()
{
	shader = new Shader(Shaders + L"052_Model.fx");

	modelCount = 1;

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
			models[i]->Scale(0.0625f, 0.0625f, 0.0625f);
		}
		else
		{
			models[i]->Scale(0.125f, 0.125f, 0.125f);
		}

		models[i]->SetShader(shader);

		D3DXVECTOR3 pos = D3DXVECTOR3((float)i * 3.0f, 0, 0);
		models[i]->Position(pos);
	}
}

TestShadow::~TestShadow()
{
	SAFE_DELETE(shader);

	for (UINT i = 0; i < modelCount; i++)
	{
		SAFE_DELETE(models[i]);
	}
	SAFE_DELETE_ARRAY(models);
	SAFE_DELETE_ARRAY(myThread);
}

void TestShadow::Update()
{
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

void TestShadow::PreRender()
{

}

void TestShadow::Render()
{
	for (UINT i = 0; i < modelCount; i++)
		models[i]->Render();
}

void TestShadow::PostRender()
{

}


