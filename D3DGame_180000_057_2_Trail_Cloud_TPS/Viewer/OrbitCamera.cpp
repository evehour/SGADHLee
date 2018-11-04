#include "stdafx.h"
#include "OrbitCamera.h"

OrbitCamera::OrbitCamera(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
	, targetPos(NULL)
{
	tpsRotation = D3DXVECTOR3(0, 0, 0);
	initPos = D3DXVECTOR3(0, 0, 0);
	tpsDistance = 15.0f;
	firstUpdate = true;
}

OrbitCamera::~OrbitCamera()
{
}

void OrbitCamera::Update()
{
	D3DXVECTOR3 forward, right, up;

	Forward(&forward);
	Right(&right);
	Up(&up);


	D3DXVECTOR3 position;
	Position(&position);

	if (Keyboard::Get()->Press('W'))
	{
		tpsDistance -= 10.0f * Time::Get()->Delta();
	}
	else if (Keyboard::Get()->Press('S'))
	{
		tpsDistance += 10.0f * Time::Get()->Delta();
	}

	if (Mouse::Get()->Press(1))
	{
		D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();
		tpsRotation.y += -val.x * Time::Get()->Delta();
		tpsRotation.x += val.y * Time::Get()->Delta();
	}

	D3DXMATRIX matCam;
	D3DXMatrixTranslation(&matCam, 0, 0, -tpsDistance);

	D3DXMATRIX matTarget;
	D3DXMatrixTranslation(&matTarget, targetPos->x, targetPos->y, targetPos->z);

	D3DXMATRIX matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot, tpsRotation.y, tpsRotation.x, 0.0f);
	matRot = matRot * matTarget;

	D3DXMATRIX matFin;
	matFin = matCam * matRot;

	position.x = matFin._41;
	position.y = matFin._42;
	position.z = matFin._43;

	Rotation(tpsRotation.x, tpsRotation.y);

	Position(position.x, position.y, position.z);

}
