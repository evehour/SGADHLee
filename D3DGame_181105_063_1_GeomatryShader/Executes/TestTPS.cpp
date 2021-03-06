#include "stdafx.h"
#include "TestTPS.h"

#include "../Objects/Player.h"
#include "../Objects/Monster.h"
#include "../Objects/GameAnimModel.h"

#include "../Model/ModelClip.h"

#include "../Components/CapsuleCollider.h"

#include "../Lights/Shadow.h"
#include "../Objects/MeshPlane.h"

TestTPS::TestTPS(ExecuteValues * values)
	: Execute(values)
	, monsterCount(10)
{
	float _scale = 1.0f / 32.0f;
	modelShader = new Shader(Shaders + L"035_Animation_HW.hlsl");

	shadow = new Shadow(values);

	player = new Player(
		Models + L"Paladin/", L"Mesh.material",
		Models + L"Paladin/", L"Mesh.mesh",
		modelShader
	);
	
	monster = new Monster*[monsterCount];
	for (UINT i = 0; i < monsterCount; i++)
		monster[i] = NULL;

	player->GetModel()->Scale(_scale, _scale, _scale);
	player->GetModel()->Position(-3, 0, 0);
	player->EnableUnit();

	D3DXMATRIX m;
	D3DXMatrixRotationY(&m, Math::ToRadian(180.0f));
	for (UINT i = 0; i < monsterCount; i++)
	{
		monster[i] = new Monster(
			Models + L"Vanguard/", L"Mesh.material",
			Models + L"Vanguard/", L"Mesh.mesh",
			modelShader
		);

		monster[i]->SetPlayer(player);
		monster[i]->GetModel()->RootAxis(m);
		//monster->GetModel()->Rotation(0, -Math::ToRadian(90.0f), 0);
		monster[i]->GetModel()->Scale(_scale, _scale, _scale);
		monster[i]->GetModel()->Position(3.0f * ((float)i + 1.0f), 0, 0);
		monster[i]->EnableUnit();
	}

	monster[1]->GetModel()->Position(6, 0, 0);


	// Mesh
	{
		plane = new MeshPlane();
		plane->SetShader(objectShader);
		plane->SetDiffuseMap(Textures + L"White.png");
		// 플레인이 안나오는 이유는 카메라 높이가 지금 0이라서 그런겅미.
		plane->Scale(10, 1, 10);
		plane->SetDiffuse(1, 1, 1, 1);
	}
}

TestTPS::~TestTPS()
{
	SAFE_DELETE(modelShader);

	SAFE_DELETE(shadow);

	SAFE_DELETE(plane);
	SAFE_DELETE(player);

	for (UINT i = 0; i < monsterCount; i++)
		SAFE_DELETE(monster[i]);
	SAFE_DELETE_ARRAY(monster);
}

void TestTPS::Update()
{
	plane->Update();
	PlayerController();
	MonsterController();

	shadow->Add(plane);
	shadow->Add(player->GetModel());
	for (UINT i = 0; i < monsterCount; i++)
	{
		if (monster[i] && monster[i]->IsEnable())
			shadow->Add(monster[i]->GetModel());
	}

	shadow->Update();
}

void TestTPS::PreRender()
{
	shadow->PreRender();
}

void TestTPS::Render()
{
	shadow->Render();
	//player->Render();

	//for (UINT i = 0; i < monsterCount; i++)
	//{
	//	if (monster[i] && monster[i]->IsEnable())
	//		monster[i]->Render();
	//}
	
}

void TestTPS::PostRender()
{
}

void TestTPS::PlayerController()
{
	player->Update();
	if (player->IsAttackColliderEnable())
	{
		for (UINT i = 0; i < monsterCount; i++)
		{
			if (!monster[i] || !monster[i]->IsEnable()) continue;
			if (monster[i]->GetCurrentState() == Unit::Unit_State::Dying) continue;

			CapsuleCollider* pPlayerAttackCollider = reinterpret_cast<CapsuleCollider*>(player->GetAttackCollider());
			CapsuleCollider* pMonsterCollider = reinterpret_cast<CapsuleCollider*>(monster[i]->GetCollider());

			if (pPlayerAttackCollider->IsCollisionOtherCapsule(pMonsterCollider))
			{
				player->OnDisableCollider();
				monster[i]->Hit();
				break;
			}

		}
	}
}

void TestTPS::MonsterController()
{
	for (UINT i = 0; i < monsterCount; i++)
	{
		if (!monster[i] || !monster[i]->IsEnable()) continue;
		monster[i]->Update();

		if (monster[i]->IsAttackColliderEnable())
		{
			CapsuleCollider* pMonsterAttackCollider = reinterpret_cast<CapsuleCollider*>(monster[i]->GetAttackCollider());
			CapsuleCollider* pPlayerCollider = reinterpret_cast<CapsuleCollider*>(player->GetCollider());

			if (pMonsterAttackCollider->IsCollisionOtherCapsule(pPlayerCollider))
			{
				monster[i]->OnDisableCollider();
				player->Hit();
			}
		}
	}
}

