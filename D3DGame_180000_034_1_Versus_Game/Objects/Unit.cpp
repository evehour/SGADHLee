#include "stdafx.h"
#include "Unit.h"

#include "../Objects/GameAnimModel.h"
#include "../Model/ModelClip.h"

#include "../Components/CapsuleCollider.h"

Unit::Unit(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, wstring shaderFolder, wstring shaderFile)
	: bEnable(false)
	, bDebugDraw(false), debugDraw(NULL)
	, isLeft(false), bSlash(false), bAttackColliderEnable(false)
	, collider(NULL), colliderBone(NULL), attackCollider(NULL), attackBone(NULL)
{
	model = new GameAnimModel(matFolder, matFile, meshFolder, meshFile);
	shader = new Shader(shaderFolder + shaderFile);
	model->SetShader(shader);

	UnitSetting();
}

Unit::Unit(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, Shader * shader)
	: bEnable(false)
	, bDebugDraw(false), debugDraw(NULL)
	, isLeft(false), bSlash(false), bAttackColliderEnable(false)
	, collider(NULL), colliderBone(NULL), attackCollider(NULL), attackBone(NULL)
	, shader(NULL)
{
	model = new GameAnimModel(matFolder, matFile, meshFolder, meshFile);
	model->SetShader(shader);

	UnitSetting();
}

void Unit::UnitSetting()
{
	state_max = static_cast<int>(Unit::Unit_State::Max);
	prevState = Unit_State::Idle;
	currentState = Unit_State::Idle;

	clips = new ModelClip*[state_max];
	for (int i = 0; i < state_max; i++)
	{
		clips[i] = NULL;
	}

	debugDraw = new DebugDraw(DebugDraw::DRAW_OBJECT_TYPE_SPHERE);
}

Unit::~Unit()
{
	SAFE_DELETE(debugDraw);
	SAFE_DELETE(attackCollider);
	SAFE_DELETE(collider);
	SAFE_DELETE(shader);

	for (int i = 0; i < (int)Unit_State::Max; i++)
	{
		SAFE_DELETE(clips[i]);
	}
	SAFE_DELETE_ARRAY(clips);
}

void Unit::Update()
{
	model->Update(true);
	if (collider)
	{
		D3DXMATRIX R, T, F;
		//D3DXVECTOR3 r = model->Rotation();
		//D3DXVECTOR3 t = model->Position();
		//D3DXMatrixRotationYawPitchRoll(&R, r.y, r.x, r.z);
		//D3DXMatrixTranslation(&T, t.x, t.y, t.z);
		//F = R * T;
		
		//F = model->World();
		F = colliderBone->Global() * model->GetBoneTransforms()[colliderBone->Index()];

		collider->Update(&F);
	}

	if (attackCollider && attackBone)
	{
		//D3DXMATRIX R, T, F;
		D3DXMATRIX m = attackBone->Global() * model->GetBoneTransforms()[attackBone->Index()];
		attackCollider->Update(&m);
	}
}

void Unit::Render()
{
	model->Render();
}

void Unit::Hit()
{
}

void Unit::InitUnit()
{
	prevState = Unit_State::Max;
	currentState = Unit_State::Max;
}

void Unit::EnableUnit()
{
	bEnable = true;
}

void Unit::DisableUnit()
{
	bEnable = false;
}

void Unit::UpdateClip()
{
	for (int i = 0; i < state_max; i++)
	{
		if (clips[i] != NULL)
		{
			model->AddClip(clips[i]);
		}
	}
}

void Unit::RestoreDiffuse()
{
	model->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
}

float Unit::GetDistanceOtherUnit(Unit * toUnit)
{
	D3DXVECTOR2 fromPos;
	fromPos.x = GetModel()->Position().x;
	fromPos.y = GetModel()->Position().z;

	D3DXVECTOR2 toPos;
	toPos.x = toUnit->GetModel()->Position().x;
	toPos.y = toUnit->GetModel()->Position().z;

	D3DXVECTOR2 dist;
	dist = toPos - fromPos;

	return D3DXVec2Length(&dist);
}
