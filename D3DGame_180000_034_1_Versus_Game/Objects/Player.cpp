#include "stdafx.h"
#include "Player.h"

#include "../Model/ModelClip.h"
#include "../Objects/GameAnimModel.h"

#include "../Components/CapsuleCollider.h"

Player::Player(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, wstring shaderFolder, wstring shaderFile)
	: Unit(matFolder, matFile, meshFolder, meshFile, shaderFolder, shaderFile)
{
	model->Rotation(0.0f, Math::ToRadian(270.0f), 0.0f);
	ColliderSetting();
	StatusSetting();
	AnimationSetting();
}

Player::Player(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, Shader * shader)
	: Unit(matFolder, matFile, meshFolder, meshFile, shader)
{
	model->Rotation(0.0f, Math::ToRadian(270.0f), 0.0f);
	ColliderSetting();
	StatusSetting();
	AnimationSetting();
}

Player::~Player()
{
}

void Player::Update()
{
	if (!this->bHit)
	{
		if (!this->bSlash)
			Move();
		Slash();
	}

	prevState = currentState;
	__super::Update();
}

void Player::Render()
{
	__super::Render();
	if (collider)
	{
		collider->Render();
	}

	if (attackCollider)
	{
		attackCollider->Render();
	}
}

void Player::AnimationSetting()
{
	int _idle, _moving, _attack, _hit, _dying;
	_idle = static_cast<int>(Unit::Unit_State::Idle);
	_moving = static_cast<int>(Unit::Unit_State::Moving);
	_attack = static_cast<int>(Unit::Unit_State::Attack);
	_hit = static_cast<int>(Unit::Unit_State::Hit);
	_dying = static_cast<int>(Unit::Unit_State::Dying);

	clips[_idle] = new ModelClip(Models + L"Paladin/Idle/Idle.anim");
	clips[_moving] = new ModelClip(Models + L"Paladin/Running/Running.anim");
	clips[_attack] = new ModelClip(Models + L"Paladin/Slash/Slash.anim");
	clips[_hit] = new ModelClip(Models + L"Paladin/Hit/Hit.anim");
	clips[_dying] = new ModelClip(Models + L"Paladin/Dying/Dying.anim");

	// Event
	clips[_attack]->TriggerRegister(44, bind(&Player::OnSlashEnd, this));
	clips[_attack]->TriggerRegister(17, bind(&Player::OnEnableCollider, this));
	clips[_attack]->TriggerRegister(38, bind(&Player::OnDisableCollider, this));
	clips[_hit]->TriggerRegister(20, bind(&Player::OnHitEnd, this));

	UpdateClip();

	GetModel()->Play(clips[_idle], true, 0.0f, 20.0f);
}

void Player::ColliderSetting()
{
#if false
	collider = new CapsuleCollider();
	collider->Position(0, 85, 0);
	collider->Scale(50, 120, 1.0f);

	attackCollider = new CapsuleCollider();
	attackCollider->Position(-12.0f, -4.0f, -48.0f);
	attackCollider->Rotation(Math::ToRadian(90.0f - 3.0f), Math::ToRadian(10.0f), Math::ToRadian(-4.0f));
	attackCollider->Scale(10.0f, 60.0f, 1.0f);
#else
	collider = new CapsuleCollider();
	//collider->Position(0, 85, 0);
	collider->Position(0, 0, 0);
	collider->Scale(60, 100, 1.0f);

	attackCollider = new CapsuleCollider();
	attackCollider->Position(-12.0f, -4.0f, -48.0f);
	attackCollider->Rotation(Math::ToRadian(90.0f - 3.0f), Math::ToRadian(10.0f), Math::ToRadian(-4.0f));
	attackCollider->Scale(10.0f, 60.0f, 1.0f);


	ModelBone* aBone = model->GetModel()->BoneByName(L"Sword_joint");
	ModelBone* mBone = model->GetModel()->BoneByIndex(0);
	SetColliderBone(mBone);
	SetAttackBone(aBone);
#endif
}

void Player::StatusSetting()
{
	this->unitStatus.Hp = 30.0f;
	this->unitStatus.Atk = 10.0f;
}

void Player::OnSlashEnd()
{
	this->bSlash = false;
}

void Player::OnHitEnd()
{
	this->bHit = false;
}

void Player::OnEnableCollider()
{
	this->bAttackColliderEnable = true;
}

void Player::OnDisableCollider()
{
	this->bAttackColliderEnable = false;
}

void Player::Move()
{
	bool bMove = false;

	if (Keyboard::Get()->Press(VK_LEFT))
	{
		isLeft = true;
		bMove = true;
	}
	else if (Keyboard::Get()->Press(VK_RIGHT))
	{
		isLeft = false;
		bMove = true;
	}

	D3DXVECTOR3 rot = model->Rotation();
	D3DXVECTOR3 lRot = rot;
	lRot.y = isLeft ? Math::ToRadian(90.0f) : Math::ToRadian(270.0f);

	D3DXVec3Lerp(&lRot, &rot, &lRot, 7.0f * Time::Delta());
	model->Rotation(lRot);

	if (bMove)
	{
		if (prevState != Unit::Unit_State::Moving)
		{
			model->Play(clips[(int)Unit::Unit_State::Moving], true, 10.0f, 20.0f);
		}

		currentState = Unit::Unit_State::Moving;

		D3DXVECTOR3 pos = model->Position();
		pos.x += (isLeft ? -1.0f : 1.0f) * 10.0f * Time::Delta();
		model->Position(pos);
	}
	else
	{
		if (prevState != Unit::Unit_State::Idle)
		{
			model->Play(clips[(int)Unit::Unit_State::Idle], true, 10.0f, 20.0f);
		}

		currentState = Unit::Unit_State::Idle;
	}
}

void Player::Slash()
{
	if (Keyboard::Get()->Down(VK_SPACE))
	{
		if (!this->bSlash)
		{
			this->bSlash = true;
			if (prevState != Unit::Unit_State::Attack)
			{
				model->Play(clips[(int)Unit::Unit_State::Attack], true, 5.0f, 20.0f);
			}

			currentState = Unit::Unit_State::Attack;
		}
	}
}

void Player::Hit()
{
	this->bHit = true;
	this->bSlash = false;
	model->Play(clips[(int)Unit::Unit_State::Hit], true, 5.0f, 20.0f);
	currentState = Unit::Unit_State::Hit;
}


