#include "stdafx.h"
#include "OrbitCamera.h"

OrbitCamera::OrbitCamera(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
	, targetPos(nullptr)
{
	tpsRotation = D3DXVECTOR3(0, 0, 0);
	initPos = D3DXVECTOR3(0, 0, 0);
	tpsDistance = 35.0f;
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

#if false
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
#else

	//이동처리
	{
		if (Mouse::Get()->Press(1))
		{
			if (Keyboard::Get()->Press('W'))
				position += forward * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('S'))
				position += -forward * moveSpeed * Time::Delta();

			if (Keyboard::Get()->Press('A'))
				position += -right * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('D'))
				position += right * moveSpeed * Time::Delta();

			if (Keyboard::Get()->Press('E'))
				position += up * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('Q'))
				position += -up * moveSpeed * Time::Delta();

		}

		if (targetPos != nullptr)
		{
			float rotY = rotation.y;
			rotY += Math::PI * 0.5f;
			rotY = -rotY;

			//position.x = (targetPos->x) + tpsDistance * sinf(rotY) * cosf(-rotation.x + Math::PI * 0.5f);
			//position.z = (targetPos->z) + tpsDistance * sinf(rotY) * sinf(-rotation.x + Math::PI * 0.5f);
			//position.y = (targetPos->y) + tpsDistance * cosf(rotY);

			position.x = (targetPos->x) + tpsDistance * cosf(rotY);
			position.z = (targetPos->z) + tpsDistance * sinf(rotY) * cosf(-rotation.x);
			position.y = (targetPos->y) + tpsDistance * sinf(rotY) * sinf(-rotation.x);
		}

		Position(position.x, position.y, position.z);
	}
#endif
}

void OrbitCamera::SetRotation(D3DXVECTOR2 angle)
{
	rotation.y = angle.x;
	rotation.x = angle.y;
	
	Rotation();
}

void OrbitCamera::SetDeltaRotation(D3DXVECTOR2 delta)
{
	rotation.y += delta.x;
	rotation.x += delta.y;

	Rotation();
}
