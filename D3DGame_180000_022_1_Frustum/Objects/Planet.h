#pragma once
#include "GameModel.h"

class Planet : public GameModel
{
public:
	enum INFORMATION_FLAG
	{
		PIF_REVOLUTION_SPEED = 0,
		PIF_ROTATION_SPEED,
		PIF_REVOLUTION_DISTANCE,
		PIF_ROTATION_AXIS,
		PIF_REVOLUTION_AXIS,
		PIF_MAX
	};

public:
	Planet(D3DXVECTOR3& vRevolutionAxis = D3DXVECTOR3(0, 0, 0), D3DXVECTOR3& vRotationAxis = D3DXVECTOR3(0, 0, 0)
		, float RevolutionSpeed = 0, float RotationSpeed = 0
		, float RevolutionDistance = 0);
	~Planet();

	void Update();
	void Render();

public:
	void SetInfo(const INFORMATION_FLAG& flag, const float* val);
	const float* GetInfo(const INFORMATION_FLAG& flag);

	Transform* GetRootTransform() const { return tRoot; }

protected:
	float RevolutionSpeed;
	float RotationSpeed;
	float RevolutionDistance;

	D3DXVECTOR3 vRevolutionAxis;
	D3DXVECTOR3 vStoreRevolutionAxis;

	D3DXVECTOR3 vRotationAxis;
	D3DXVECTOR3 vStoreRotationAxis;

	D3DXMATRIX matWorld;

	Transform *tRoot;
	Transform *tRotationAxis;
	Transform *tRevolutionAxis, *tRevolution;

private:
	void SettingRotation(); // 자전
	void SettingRevolution(); // 공전
};