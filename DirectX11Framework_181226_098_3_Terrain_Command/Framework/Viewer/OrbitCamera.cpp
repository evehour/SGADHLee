#include "Framework.h"
#include "OrbitCamera.h"

OrbitCamera::OrbitCamera(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
	, targetPos(nullptr)
{
	tpsRotation = D3DXVECTOR3(0, 0, 0);
	initPos = D3DXVECTOR3(0, 0, 0);
	tpsDistance = 5.0f;
	firstUpdate = true;
	orbitRotationX = (float)D3DX_PI;
	orbitTheta = (float)(D3DX_PI * 0.5f);
	orbitPhi = 0.0f;
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

	D3DXVECTOR3 pos;
	Position(&pos);

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
				pos += forward * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('S'))
				pos += -forward * moveSpeed * Time::Delta();

			if (Keyboard::Get()->Press('A'))
				pos += -right * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('D'))
				pos += right * moveSpeed * Time::Delta();

			if (Keyboard::Get()->Press('E'))
				pos += up * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('Q'))
				pos += -up * moveSpeed * Time::Delta();

		}

		if (targetPos != nullptr)
		{
			float rotationY = orbitPhi - (Math::PI * 0.5f);
			pos.x = (targetPos->x) + tpsDistance * sinf(orbitTheta) * cosf(rotationY);
			pos.z = (targetPos->z) + tpsDistance * sinf(orbitTheta) * sinf(rotationY);
			pos.y = (targetPos->y) + tpsDistance * cosf(orbitTheta);
		}

		Position(pos.x, pos.y, pos.z);
	}
#endif
}

void OrbitCamera::SetRotation(D3DXVECTOR2 angle)
{
	orbitRotationX = angle.x - (float)D3DX_PI;
	orbitTheta = angle.y + (float)(D3DX_PI * 0.5f);

	SetRotationHelper();
}

void OrbitCamera::SetDeltaRotation(D3DXVECTOR2 delta)
{
	orbitRotationX += delta.x;
	orbitTheta -= delta.y;

	SetRotationHelper();
}

void OrbitCamera::SetRotationHelper()
{
	orbitPhi = -(orbitRotationX - Math::PI);

	Rotation
	(
		-(orbitTheta - Math::PI * 0.5f),
		orbitRotationX - Math::PI
	);
}
