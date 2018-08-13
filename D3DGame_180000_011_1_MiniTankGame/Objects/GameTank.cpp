#include "stdafx.h"
#include "GameTank.h"

GameTank::GameTank(wstring matFolder, wstring matFile)
	: GameModel(matFolder, matFile, Models + L"Tank/", L"Tank.mesh")
	, wheelState(TANK_STATE_IDLE)
	, turretAngle(0.0f)
	, WheelDir(1.0f)
{
	wheelBone[0] = model->BoneByName(L"l_front_wheel_geo");
	wheelBone[1] = model->BoneByName(L"r_front_wheel_geo");
	wheelBone[2] = model->BoneByName(L"l_back_wheel_geo");
	wheelBone[3] = model->BoneByName(L"r_back_wheel_geo");

	for (int i = 0; i < 4; i++)
		wheelWorld[i] = wheelBone[i]->Local();

	turretBone = model->BoneByName(L"turret_geo");
	turretWorld = turretBone->Local();
}

GameTank::~GameTank()
{
}

void GameTank::Update()
{
	__super::Update();

	D3DXMATRIX R;
	switch (wheelState)
	{
	case TANK_STATE_IDLE:
		break;
	case TANK_STATE_MOVE:
		D3DXMatrixRotationX(&R, Math::ToRadian(Time::Get()->Running() * -100 * WheelDir));
		for (int i = 0; i < 4; i++)
			wheelBone[i]->Local(R * wheelWorld[i]);
		break;
	}
}

void GameTank::Render()
{
	__super::Render();
}

void GameTank::SetTankState(TANK_STATE state)
{
	switch (state)
	{
	case TANK_STATE_IDLE:
		wheelState = state;
		break;
	case TANK_STATE_MOVE:
		wheelState = state;
		break;
	case TANK_STATE_ATTACK:
		break;
	}
}

void GameTank::RotateTurret(float amount)
{
#if false
	//turretAngle = amount;
	//if (turretAngle > 2 * Math::PI) turretAngle -= 2 * Math::PI;
	//D3DXMATRIX R;
	//D3DXMatrixRotationY(&R, turretAngle);
	//D3DXMatrixRotationX(&R, sinf(Time::Get()->Running() * 2.0f) * 0.25f);
	//turretBone->Local(R * turretWorld);


	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, sinf(Time::Get()->Running() * 2.0f) *0.25f);
	
	D3DXMATRIX L = turretBone->Local();
	turretBone->Local(L * R);
#else
	turretAngle += amount;
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, turretAngle);
	turretBone->Local(R * turretWorld);
#endif
}

void GameTank::SetWheelDir(bool isFoward)
{
	WheelDir = (isFoward) ? 1.0f : -1.0f;
}

D3DXMATRIX GameTank::GetTurretMatrix()
{
	D3DXMATRIX temp;

	temp = turretBone->Global() * World();
	//temp = turretBone->Local() * World();

	return temp;
}
