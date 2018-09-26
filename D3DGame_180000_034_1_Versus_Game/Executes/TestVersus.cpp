#include "stdafx.h"
#include "TestVersus.h"

#include "../Objects/Player.h"
#include "../Objects/Monster.h"
#include "../Objects/GameAnimModel.h"

#include "../Model/ModelClip.h"

#include "../Components/CapsuleCollider.h"

TestVersus::TestVersus(ExecuteValues * values)
	: Execute(values)
{
	float _scale = 1.0f / 32.0f;
	shader = new Shader(Shaders + L"035_Animation_HW.hlsl");

	player = new Player(
		Models + L"Paladin/", L"Mesh.material",
		Models + L"Paladin/", L"Mesh.mesh",
		shader
	);

	monster = new Monster(
		Models + L"Vanguard/", L"Mesh.material",
		Models + L"Vanguard/", L"Mesh.mesh",
		shader
	);
	monster->SetPlayer(player);
	D3DXMATRIX m;
	D3DXMatrixRotationY(&m, Math::ToRadian(180.0f));
	monster->GetModel()->RootAxis(m);
	//monster->GetModel()->Rotation(0, -Math::ToRadian(90.0f), 0);

	// Setting Clips
	{
		for (UINT i = 0; i < 1000; i++)
		{
			playerClips[i] = NULL;
			monsterClips[i] = NULL;
		}

		int _idle, _moving, _attack, _dying;
		_idle = static_cast<int>(Unit::Unit_State::Idle);
		_moving = static_cast<int>(Unit::Unit_State::Moving);
		_attack = static_cast<int>(Unit::Unit_State::Attack);
		_dying = static_cast<int>(Unit::Unit_State::Dying);
		//Player
		{
			playerClips[_idle] = new ModelClip(Models + L"Paladin/Idle/Idle.anim");
			playerClips[_moving] = new ModelClip(Models + L"Paladin/Running/Running.anim");
			playerClips[_attack] = new ModelClip(Models + L"Paladin/Slash/Slash.anim");

			// Event
			playerClips[_attack]->TriggerRegister(44, bind(&Player::OnSlashEnd, player));
			playerClips[_attack]->TriggerRegister(17, bind(&Player::OnEnableCollider, player));
			playerClips[_attack]->TriggerRegister(38, bind(&Player::OnDisableCollider, player));

			player->SetClip(Unit::Unit_State::Idle, playerClips[_idle]);
			player->SetClip(Unit::Unit_State::Moving, playerClips[_moving]);
			player->SetClip(Unit::Unit_State::Attack, playerClips[_attack]);
			player->UpdateClip();

			player->GetModel()->Play(playerClips[_idle], true, 0.0f, 20.0f);

			ModelBone* bone = player->GetModel()->GetModel()->BoneByName(L"Sword_joint");
			player->SetAttackBone(bone);
		}

		//Monster
		{
			monsterClips[_idle] = new ModelClip(Models + L"Vanguard/Idle/Idle.anim");
			monsterClips[_moving] = new ModelClip(Models + L"Vanguard/Walking/Walking.anim");
			monsterClips[_attack] = new ModelClip(Models + L"Vanguard/Punch/Punch.anim");
			monsterClips[_dying] = new ModelClip(Models + L"Vanguard/Dying/Dying.anim");

			// Event
			monsterClips[_attack]->TriggerRegister(76, bind(&Monster::OnSlashEnd, monster));

			monster->SetClip(Unit::Unit_State::Idle, monsterClips[_idle]);
			monster->SetClip(Unit::Unit_State::Moving, monsterClips[_moving]);
			monster->SetClip(Unit::Unit_State::Attack, monsterClips[_attack]);
			monster->SetClip(Unit::Unit_State::Dying, monsterClips[_dying]);
			monster->UpdateClip();

			monster->GetModel()->Play(monsterClips[_idle], true, 0.0f, 20.0f);
		}
	}


	player->GetModel()->Scale(_scale, _scale, _scale);
	player->GetModel()->Position(-3, 0, 0);
	monster->GetModel()->Scale(_scale, _scale, _scale);
	monster->GetModel()->Position(3, 0, 0);

	player->EnableUnit();
	monster->EnableUnit();
}

TestVersus::~TestVersus()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(player);
	SAFE_DELETE(monster);

	for (UINT i = 0; i < 1000; i++)
	{
		SAFE_DELETE(playerClips[i]);
		SAFE_DELETE(monsterClips[i]);
	}
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
	if (monster->IsEnable()) monster->Render();
}

void TestVersus::PostRender()
{
}

void TestVersus::PlayerController()
{
	player->Update();
	if (player->IsAttackColliderEnable())
	{
		if (!monster->IsEnable()) return;
		if (monster->GetCurrentState() == Unit::Unit_State::Dying) return;

		CapsuleCollider* pPlayerAttackCollider = reinterpret_cast<CapsuleCollider*>(player->GetAttackCollider());
		CapsuleCollider* pMonsterCollider = reinterpret_cast<CapsuleCollider*>(monster->GetCollider());

		if (pPlayerAttackCollider->IsCollisionWithCapsule(pMonsterCollider))
		{
			player->OnDisableCollider();
			monster->GetModel()->SetDiffuse(1.0f, 0.0f, 0.0f, 1.0f);
			Time::Get()->AddInvoker(0.25f, bind(&Monster::RestoreDiffuse, monster));

			// Calculate status
			monster->unitStatus.Hp -= player->unitStatus.Atk;
		}
	}
}

void TestVersus::MonsterController()
{
	if (monster->IsEnable())
	{
		monster->Update();
	}
}

