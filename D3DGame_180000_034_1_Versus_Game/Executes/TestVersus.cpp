#include "stdafx.h"
#include "TestVersus.h"

#include "../Objects/Player.h"
#include "../Objects/Monster.h"
#include "../Objects/GameAnimModel.h"

#include "../Model/ModelClip.h"

#include "../Components/CapsuleCollider.h"

TestVersus::TestVersus(ExecuteValues * values)
	: Execute(values)
	, monsterCount(100)
{
	float _scale = 1.0f / 32.0f;
	shader = new Shader(Shaders + L"035_Animation_HW.hlsl");

	player = new Player(
		Models + L"Paladin/", L"Mesh.material",
		Models + L"Paladin/", L"Mesh.mesh",
		shader
	);
	
	monster = new Monster*[monsterCount];
	for (UINT i = 0; i < monsterCount; i++)
		monster[i] = NULL;

	player->GetModel()->Scale(_scale, _scale, _scale);
	player->GetModel()->Position(-3, 0, 0);
	player->EnableUnit();

	D3DXMATRIX m;
	D3DXMatrixRotationY(&m, Math::ToRadian(180.0f));
	for (UINT i = 0; i < 2; i++)
	{
		monster[i] = new Monster(
			Models + L"Vanguard/", L"Mesh.material",
			Models + L"Vanguard/", L"Mesh.mesh",
			shader
		);

		monster[i]->SetPlayer(player);
		monster[i]->GetModel()->RootAxis(m);
		//monster->GetModel()->Rotation(0, -Math::ToRadian(90.0f), 0);
		monster[i]->GetModel()->Scale(_scale, _scale, _scale);
		monster[i]->GetModel()->Position(3, 0, 0);
		monster[i]->EnableUnit();
	}

	monster[1]->GetModel()->Position(6, 0, 0);
}

TestVersus::~TestVersus()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(player);

	for (UINT i = 0; i < monsterCount; i++)
		SAFE_DELETE(monster[i]);
	SAFE_DELETE_ARRAY(monster);
}

void TestVersus::Update()
{
	PlayerController();
	MonsterController();
}

void TestVersus::PreRender()
{

}

void TestVersus::Render()
{
	player->Render();

	for (UINT i = 0; i < monsterCount; i++)
	{
		if (monster[i] && monster[i]->IsEnable())
			monster[i]->Render();
	}
	
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
				monster[i]->GetModel()->SetDiffuse(1.0f, 0.0f, 0.0f, 1.0f);
				Time::Get()->AddInvoker(0.25f, bind(&Monster::RestoreDiffuse, monster[i]));

				// Calculate status
				monster[i]->unitStatus.Hp -= player->unitStatus.Atk;
				break;
			}

		}
	}
}

void TestVersus::MonsterController()
{
	for (UINT i = 0; i < monsterCount; i++)
	{
		if (!monster[i] || !monster[i]->IsEnable()) continue;
		monster[i]->Update();
	}
}

