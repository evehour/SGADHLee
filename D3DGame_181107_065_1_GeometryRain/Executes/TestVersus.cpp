#include "stdafx.h"
#include "TestVersus.h"

#include "../Objects/Player.h"
#include "../Objects/Monster.h"
#include "../Objects/GameAnimModel.h"

#include "../Model/ModelClip.h"
#include "../Model/ModelBone.h"

#include "../Components/CapsuleCollider.h"

#include "../Lights/Shadow.h"
#include "../Objects/MeshPlane.h"

#include "../Renders/TrailRender.h"
#include "../Objects/DebugDraw.h"

#include "../Physics/LineMake.h"

TestVersus::TestVersus(ExecuteValues * values)
	: Execute(values)
	, monsterCount(2)
{
	float _scale = 1.0f / 32.0f;
	modelShader = new Shader(Shaders + L"035_Animation_HW.hlsl");

	shadow = new Shadow(values);

	player = new Player(
		Models + L"Paladin/", L"Mesh2.material",
		Models + L"Paladin/", L"Mesh2.mesh",
		modelShader
	);
	
	monster = new Monster*[monsterCount];
	for (UINT i = 0; i < monsterCount; i++)
		monster[i] = NULL;

	player->GetModel()->Scale(_scale, _scale, _scale);
	player->GetModel()->Position(-3, 0, 0);
	player->EnableUnit();

	idx = player->GetModel()->GetModel()->BoneByName(L"Sword_joint")->Index();


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
		// �÷����� �ȳ����� ������ ī�޶� ���̰� ���� 0�̶� �׷��Ϲ�.
		plane->Scale(10, 1, 10);
		plane->SetDiffuse(1, 1, 1, 1);
	}

	rayLine = new LineMake();
	rayLine->AddLine(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, -1000));
	rayLine->SetColor(D3DXCOLOR(1, 0, 0, 1));
	rayLine->UpdateBuffer();
}

TestVersus::~TestVersus()
{
	SAFE_DELETE(modelShader);

	SAFE_DELETE(shadow);

	SAFE_DELETE(plane);
	SAFE_DELETE(player);

	for (UINT i = 0; i < monsterCount; i++)
		SAFE_DELETE(monster[i]);
	SAFE_DELETE_ARRAY(monster);
}

void TestVersus::Update()
{
	plane->Update();
	PlayerController();
	MonsterController();

	//shadow->Add(plane);
	//shadow->Add(player->GetModel());
	//for (UINT i = 0; i < monsterCount; i++)
	//{
	//	if (monster[i] && monster[i]->IsEnable())
	//		shadow->Add(monster[i]->GetModel());
	//}

	//shadow->Update();
}

void TestVersus::PreRender()
{
	shadow->PreRender();
}

void TestVersus::Render()
{
#if false
	shadow->Render();
#else
	player->Render();

	for (UINT i = 0; i < monsterCount; i++)
	{
		if (monster[i] && monster[i]->IsEnable())
			monster[i]->Render();
	}
#endif
	player->trailRender->Render();
	rayLine->Render();

	if (rayCollision)
		ImGui::Text("collision: Collision.");
	else
		ImGui::Text("collision: No Collision.");
}

void TestVersus::PostRender()
{
}

void TestVersus::PlayerController()
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

			if (pPlayerAttackCollider->IsCollisionWithCapsule(pMonsterCollider))
			{
				player->OnDisableCollider();
				monster[i]->Hit();
				break;
			}

		}
	}

	D3DXVECTOR3 pos = player->GetModel()->Position();
	D3DXVECTOR3 dir = -player->GetModel()->Direction();
	D3DXVec3Normalize(&dir, &dir); // MUST HAVE

	rayCollision = false;
	for (UINT i = 0; i < monsterCount; i++)
	{
		if (!monster[i] || !monster[i]->IsEnable()) continue;
		if (monster[i]->GetCurrentState() == Unit::Unit_State::Dying) continue;
		if (monster[i]->GetCurrentState() == Unit::Unit_State::Hit) continue;

		CapsuleCollider* pMonsterCollider = reinterpret_cast<CapsuleCollider*>(monster[i]->GetCollider());
		bool bIntersect = pMonsterCollider->IsCollisionWithRay(pos, dir);
		rayCollision |= bIntersect;

		if (bIntersect)
		{
			if (player->IsAttackColliderEnable())
			{
				player->OnDisableCollider();
				monster[i]->Hit();
			}
		}
	}
	rayLine->SetWorld(player->GetModel()->World());
	rayLine->Update();

}

void TestVersus::MonsterController()
{
	for (UINT i = 0; i < monsterCount; i++)
	{
		if (!monster[i] || !monster[i]->IsEnable()) continue;
		monster[i]->Update();

		if (monster[i]->IsAttackColliderEnable())
		{
			CapsuleCollider* pMonsterAttackCollider = reinterpret_cast<CapsuleCollider*>(monster[i]->GetAttackCollider());
			CapsuleCollider* pPlayerCollider = reinterpret_cast<CapsuleCollider*>(player->GetCollider());

			if (pMonsterAttackCollider->IsCollisionWithCapsule(pPlayerCollider))
			{
				monster[i]->OnDisableCollider();
				player->Hit();
			}
		}
	}
}
