#include "stdafx.h"
#include "Monster.h"

#include "../Objects/GameAnimModel.h"
#include "../Model/ModelClip.h"

#include "../Components/CapsuleCollider.h"

Monster::Monster(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, wstring shaderFolder, wstring shaderFile)
	: Unit(matFolder, matFile, meshFolder, meshFile, shaderFolder, shaderFile)
	, unitPlayer(NULL)
{
	ColliderSetting();
	StatusSetting();
	StateSetting();
	AnimationSetting();
}

Monster::Monster(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, Shader * shader)
	: Unit(matFolder, matFile, meshFolder, meshFile, shader)
	, unitPlayer(NULL)
{
	ColliderSetting();
	StatusSetting();
	StateSetting();
	AnimationSetting();
}

Monster::~Monster()
{
}

void Monster::Update()
{
	if (currentState == Unit::Unit_State::Dying)
	{
		dyingTimer += Time::Delta();
		if (dyingTimer >= 5.0f)
		{
			this->bEnable = false;
		}
	}
	else
	{
		if (!this->bSlash && !this->bHit)
		{
			float dist = GetDistanceOtherUnit(unitPlayer);

			if (dist > _searchDistance) currentState = Unit::Unit_State::Idle;
			else currentState = Unit::Unit_State::Moving;

			if (dist < _attackDistance) currentState = Unit::Unit_State::Attack;
		}

		switch (currentState)
		{
		case Unit::Unit_State::Idle:
			Idle();
			break;
		case Unit::Unit_State::Moving:
			Move();
			break;
		case Unit::Unit_State::Attack:
			Slash();
			break;
		}

		if (unitStatus.Hp <= 0.0f)
		{
			Dying();
		}
	}

	prevState = currentState;

	__super::Update();
}

void Monster::Render()
{
	__super::Render();

	if (bDebugDraw && debugDraw)
	{
		debugDraw->Position(model->Position());

		debugDraw->SetColor(D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.0f));
		debugDraw->Scale(_searchDistance, _searchDistance, _searchDistance);
		debugDraw->Update();
		debugDraw->Render();

		debugDraw->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));
		debugDraw->Scale(_attackDistance, _attackDistance, _attackDistance);
		debugDraw->Update();
		debugDraw->Render();
	}

	if (bDebugDraw)
	{
		if (collider)
		{
			collider->Render();
		}

		if (attackCollider)
		{
			attackCollider->Render();
		}
	}
}

void Monster::Idle()
{
	if (prevState != Unit::Unit_State::Idle)
	{
		model->Play(clips[(UINT)Unit::Unit_State::Idle], true, 20.0f, 20.0f);
	}
}

void Monster::Move()
{
	if (prevState != Unit::Unit_State::Moving)
	{
		model->Play(clips[(UINT)Unit::Unit_State::Moving], true, 20.0f, 20.0f);
	}

	SetLookAt(Time::Delta() * 20.0f);
	if (!this->bSlash)
	{
		D3DXVECTOR3 pos = model->Position() + (model->Direction() * _moveSpeed * Time::Delta());
		model->Position(pos);
	}
}

void Monster::Slash()
{
	if (!bSlash)
	{
		bSlash = true;
		SetLookAt();
		if (prevState != Unit::Unit_State::Attack)
		{
			model->Play(clips[(UINT)Unit::Unit_State::Attack], true, 20.0f, 20.0f);
		}
	}
}

void Monster::Hit()
{
	this->bHit = true;
	this->bSlash = false;

	SetCurrentState(Unit::Unit_State::Hit);
	model->SetDiffuse(1.0f, 0.0f, 0.0f, 1.0f);
	Time::Get()->AddInvoker(0.25f, bind(&Monster::RestoreDiffuse, this));

	// Calculate status
	unitStatus.Hp -= unitPlayer->unitStatus.Atk;

	// Play animation
	model->Play(clips[(UINT)Unit::Unit_State::Hit], true, 20.0f, 20.0f);
}

void Monster::Dying()
{
	currentState = Unit::Unit_State::Dying;
	model->Play(clips[(UINT)Unit::Unit_State::Dying], false, 20.0f, 20.0f);
	dyingTimer = 0.0f;
}

void Monster::AnimationSetting()
{
	int _idle, _moving, _attack, _hit, _dying;
	_idle = static_cast<int>(Unit::Unit_State::Idle);
	_moving = static_cast<int>(Unit::Unit_State::Moving);
	_attack = static_cast<int>(Unit::Unit_State::Attack);
	_hit = static_cast<int>(Unit::Unit_State::Hit);
	_dying = static_cast<int>(Unit::Unit_State::Dying);

	clips[_idle] = new ModelClip(Models + L"Vanguard/Idle/Idle.anim");
	clips[_moving] = new ModelClip(Models + L"Vanguard/Walking/Walking.anim");
	clips[_attack] = new ModelClip(Models + L"Vanguard/Punch/Punch.anim");
	clips[_hit] = new ModelClip(Models + L"Vanguard/Hit/Hit.anim");
	clips[_dying] = new ModelClip(Models + L"Vanguard/Dying/Dying.anim");

	ModelClip* d = clips[_attack];

	// Event
	clips[_attack]->TriggerRegister(76, bind(&Monster::OnSlashEnd, this));
	clips[_attack]->TriggerRegister(31, bind(&Monster::OnEnableCollider, this));
	clips[_attack]->TriggerRegister(42, bind(&Monster::OnDisableCollider, this));
	clips[_hit]->TriggerRegister(40, bind(&Monster::OnHitEnd, this));

	UpdateClip();

	model->Play(clips[_idle], true, 0.0f, 20.0f);
}

void Monster::ColliderSetting()
{
	collider = new CapsuleCollider();
	//collider->Position(0, 85, 0);
	collider->Position(0, 0, 0);
	collider->Scale(60, 100, 1.0f);

	attackCollider = new CapsuleCollider();
	attackCollider->Position(-10.0f, 0.0f, 0.0f);
	//attackCollider->Rotation(Math::ToRadian(90.0f - 3.0f), Math::ToRadian(10.0f), Math::ToRadian(-4.0f));
	attackCollider->Scale(20.0f, 3.0f, 1.0f);

	//ModelBone* aBone = model->GetModel()->BoneByName(L"mixamorig:RightHand");
	ModelBone* aBone = model->GetModel()->BoneByIndex(36);
	SetAttackBone(aBone);
	ModelBone* mBone = model->GetModel()->BoneByIndex(2);
	SetColliderBone(mBone);
}

void Monster::StatusSetting()
{
	_searchDistance = 5.0f;
	_attackDistance = 3.0f;
	_moveSpeed = 20.0f;

	this->unitStatus.Hp = 30.0f;
	this->unitStatus.Atk = 0.0f;
}

void Monster::StateSetting()
{
	prevState = Unit_State::Max;
	currentState = Unit_State::Max;
}

void Monster::SetLookAt(float deltaTime)
{
	D3DXVECTOR3 mobPos = model->Position();
	D3DXVECTOR3 playerPos = unitPlayer->GetModel()->Position();

	mobPos.y = playerPos.y = 0.0f;

	D3DXVECTOR3 direction = playerPos - mobPos;

	if (D3DXVec3Length(&direction) > 0.0f)
	{
		D3DXVec3Normalize(&direction, &direction);
		D3DXQUATERNION qRot = Math::LookAt(model->Position(), playerPos, model->Up());
		D3DXVECTOR3 vRot, vdRot;
		Math::toEulerAngle(qRot, vRot);
		D3DXVec3Lerp(&vdRot, &model->Rotation(), &vRot, deltaTime);
		
		model->Rotation(vdRot);
	}
}

void Monster::OnSlashEnd()
{
	this->bSlash = false;
}

void Monster::OnEnableCollider()
{
	this->bAttackColliderEnable = true;
}

void Monster::OnDisableCollider()
{
	this->bAttackColliderEnable = false;
}

void Monster::OnHitEnd()
{
	this->bHit = false;
}
