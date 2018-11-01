#include "stdafx.h"
#include "Planet.h"

Planet::Planet(D3DXVECTOR3& vRevolutionAxis, D3DXVECTOR3& vRotationAxis, float RevolutionSpeed, float RotationSpeed, float RevolutionDistance)
	: GameModel(Materials + L"Meshes/", L"Sphere.material", Models + L"Meshes/", L"Sphere.mesh")
	, vRevolutionAxis(vRevolutionAxis), vStoreAxis(0, 0, 0)
	, parent(NULL)
	, RevolutionSpeed(RevolutionSpeed), RotationSpeed(RotationSpeed), RevolutionDistance(RevolutionDistance)
{
	worldBuffer = new WorldBuffer();
	matRevolutionAxis = new WorldBuffer();

	D3DXMatrixIdentity(&matWorld);
}

Planet::~Planet()
{
	SAFE_DELETE(matRevolutionAxis);
	SAFE_DELETE(worldBuffer);
}

void Planet::Update()
{
	SettingRotoation();

	__super::Update();

	SettingFinalWorld();
	worldBuffer->SetMatrix(matWorld);
}

void Planet::Render()
{
	__super::Render();
}

void Planet::SetParent(Planet * planet)
{
	parent = planet;
}

void Planet::SetInfo(const INFORMATION_FLAG& flag, const float& val)
{
	switch (flag)
	{
	case Planet::PIF_REVOLUTION_SPEED:
		RevolutionSpeed = val;
		break;
	case Planet::PIF_ROTATION_SPEED:
		RotationSpeed = val;
		break;
	case Planet::PIF_REVOLUTION_DISTANCE:
		RevolutionDistance = val;
		break;
	case Planet::PIF_ROTATION_AXIS:
		break;
	}
}

float Planet::GetInfo(const INFORMATION_FLAG & flag)
{
	switch (flag)
	{
	case Planet::PIF_REVOLUTION_SPEED:
		return RevolutionSpeed;

	case Planet::PIF_ROTATION_SPEED:
		return RotationSpeed;

	case Planet::PIF_REVOLUTION_DISTANCE:
		return RevolutionDistance;

	default:
		return 0.0f;
	}
}

void Planet::SettingFinalWorld()
{
}

void Planet::SettingRotoation()
{
	vStoreAxis.y = vStoreAxis.y + (Rotation().y * RotationSpeed * Time::Delta());
	RootAxis(vStoreAxis);
}
