#include "stdafx.h"
#include "TestAnimModel.h"

#include "./Objects/GameAnimModel.h"

TestAnimModel::TestAnimModel()
{
	modelCount = 1;
	kachujin = new GameAnimModel*[modelCount];
	myThread = new thread*[modelCount];

	for (UINT i = 0; i < modelCount; i++)
	{
		myThread[i] = NULL;

		kachujin[i] = new GameAnimModel
		(
			Shaders + L"052_Model.fx",
			Models + L"Soldier/", L"Soldier.material",
			Models + L"Soldier/", L"Soldier.mesh"
		);
		kachujin[i]->Scale(0.0125f, 0.0125f, 0.0125f);
		kachujin[i]->Position((float)i * 5, 0, 0);
		//kachujin[i]->AddClip(Models + L"Kachujin/Samba_Dance.anim");
		//kachujin[i]->Play((UINT)0, true, 0.0f, 20.0f);
	}
}

TestAnimModel::~TestAnimModel()
{
	for (UINT i = 0; i < modelCount; i++)
		SAFE_DELETE(kachujin[i]);
	SAFE_DELETE_ARRAY(kachujin);

	SAFE_DELETE_ARRAY(myThread);
}

void TestAnimModel::Update()
{
#if 1
	for (UINT i = 0; i < modelCount; i++)
		kachujin[i]->Update();
#else
	for (UINT i = 0; i < modelCount; i++)
	{
		myThread[i] = kachujin[i]->UpdateThread();
	}


	for (UINT i = 0; i < modelCount; i++)
	{
		if (myThread[i] && myThread[i]->joinable())
			myThread[i]->join();
	}
#endif
}

void TestAnimModel::PreRender()
{

}

void TestAnimModel::Render()
{
	for (UINT i = 0; i < modelCount; i++)
		kachujin[i]->Render();
}

void TestAnimModel::PostRender()
{

}


