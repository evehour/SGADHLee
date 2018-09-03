#include "stdafx.h"
#include "Planet.h"

Planet::Planet(D3DXVECTOR3& vRevolutionAxis, D3DXVECTOR3& vRotationAxis, float RevolutionSpeed, float RotationSpeed, float RevolutionDistance)
	: GameModel(Materials + L"Meshes/", L"Sphere.material", Models + L"Meshes/", L"Sphere.mesh")
	, vRevolutionAxis(vRevolutionAxis), vRotationAxis(0, 0, 0), vStoreRotationAxis(0, 0, 0), vStoreRevolutionAxis(0, 0, 0)
	, RevolutionSpeed(RevolutionSpeed), RotationSpeed(RotationSpeed), RevolutionDistance(RevolutionDistance)
{
	D3DXMatrixIdentity(&matWorld);

	tRotationAxis.AddChild(transform);
}

Planet::~Planet()
{
}

void Planet::Update()
{
	SettingRotation();
	SettingRevolution();

	__super::Update();
}

void Planet::Render()
{
	__super::Render();
}

void Planet::SetInfo(const INFORMATION_FLAG& flag, const float* val)
{
	switch (flag)
	{
	case Planet::PIF_REVOLUTION_SPEED:
		RevolutionSpeed = *val;
		break;
	case Planet::PIF_ROTATION_SPEED:
		RotationSpeed = *val;
		break;
	case Planet::PIF_REVOLUTION_DISTANCE:
		RevolutionDistance = *val;
		break;
	case Planet::PIF_ROTATION_AXIS:
		vRotationAxis = D3DXVECTOR3(val[0], val[1], val[2]);
		tRotationAxis.SetLocalRotation(vRotationAxis);
		break;

	case Planet::PIF_REVOLUTION_AXIS:
		vRevolutionAxis = D3DXVECTOR3(val[0], val[1], val[2]);
		break;
	}
}

const float* Planet::GetInfo(const INFORMATION_FLAG & flag)
{
	switch (flag)
	{
	case Planet::PIF_REVOLUTION_SPEED:
		return &RevolutionSpeed;

	case Planet::PIF_ROTATION_SPEED:
		return &RotationSpeed;

	case Planet::PIF_REVOLUTION_DISTANCE:
		return &RevolutionDistance;

	case Planet::PIF_ROTATION_AXIS:
		return (float*)tRotationAxis.GetAngle();

	default:
		return NULL;
	}
}

void Planet::SettingRotation()
{
	transform->RotationLocal(D3DXVECTOR3(0, Math::ToRadian(1) * RotationSpeed * Time::Delta(), 0));
}

void Planet::SettingRevolution()
{
}
