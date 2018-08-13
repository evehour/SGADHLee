#pragma once
#include "GameModel.h"

enum TANK_STATE
{
	TANK_STATE_IDLE = 0,
	TANK_STATE_MOVE,
	TANK_STATE_ATTACK,
	TANK_STATE_MAX
};

class GameTank : public GameModel
{
public:
	GameTank(wstring matFolder, wstring matFile);
	~GameTank();

	void Update() override;
	void Render() override;

private:
	ModelBone* wheelBone[4];
	D3DXMATRIX wheelWorld[4];
	TANK_STATE wheelState;
	float WheelDir;

	ModelBone* turretBone;
	D3DXMATRIX turretWorld;
	float turretAngle;
public:
	void SetTankState(TANK_STATE state);
	void RotateTurret(float amount);
	void SetWheelDir(bool isFoward);
	D3DXMATRIX GetTurretMatrix();// { return turretBone->Local() * World(); }
};