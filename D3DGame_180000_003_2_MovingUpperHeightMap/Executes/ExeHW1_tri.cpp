#include "stdafx.h"
#include "./Executes/Sphere.h"
#include "./Executes/Terrain.h"
#include "ExeHW1.h"

ExeHW1::ExeHW1(ExecuteValues * values)
	: Execute(values)
	, degree(0), radius(5)
	, dgr(0,0)
{
	m_sphere = new Sphere(values);
	m_terrain = new Terrain(values);
}

ExeHW1::~ExeHW1()
{
	SAFE_DELETE(m_sphere);
	SAFE_DELETE(m_terrain);
}

void ExeHW1::Update()
{
#if 0
	D3DXVECTOR3 _pos{ 0,0,0 };
	degree += (20.0f * Time::Get()->Delta());

	if (degree > 360.0f)
		degree -= 360.0f;

	_pos.x = cosf(Math::ToRadian(degree)) * radius;
	_pos.y = 0.0f;
	_pos.z = sinf(Math::ToRadian(degree)) * radius;

	values->MainCamera->Position(_pos.x, _pos.y, _pos.z);
#endif

#if 0
	D3DXVECTOR3 _pos{ 0,0,0 };
	degree += (20.0f * Time::Get()->Delta());

	if (degree > 360.0f)
		degree -= 360.0f;

	_pos.x = cosf(Math::ToRadian(degree)) * radius;
	_pos.y = 0.0f;
	_pos.z = sinf(Math::ToRadian(degree)) * radius;

	values->MainCamera->Position(_pos.x, _pos.y, _pos.z);

	D3DXVECTOR2 rotation;
	values->MainCamera->Rotation(&rotation);
	values->MainCamera->Rotation(rotation.x, (-degree - 90.0f) * D3DX_PI / 180.0f/*degree + 180.0f*/);
#else

#if 0
	D3DXVECTOR3 up;
	D3DXVECTOR3 camPos;
	D3DXVECTOR3 direction;
	D3DXVECTOR3 axis;
	D3DXQUATERNION quat;

	values->MainCamera->Position(&camPos);
	values->MainCamera->Up(&up);

	direction = D3DXVECTOR3(0, 0, 0) - camPos;
	D3DXVec3Normalize(&direction, &direction);

	D3DXVec3Cross(&axis, &D3DXVECTOR3(0, 0, 1), &direction);
	D3DXVec3Normalize(&axis, &axis);

	float angle = acos(D3DXVec3Dot(&D3DXVECTOR3(0, 0, 1), &direction));
	D3DXQuaternionRotationAxis(&quat, &axis, angle);

	angle = angle * 180.0f / D3DX_PI;
	if (!std::signbit(axis.y))
	{
		angle = -angle;
	}
	D3DXVECTOR2 rotation;
	values->MainCamera->Rotation(&rotation);
	//values->MainCamera->Rotation(quat.x, quat.y);
	//values->MainCamera->Rotation(cosf(angle), sinf(angle));
	values->MainCamera->RotationDegree(rotation.x, -angle);
#else
	D3DXQUATERNION quat;
	D3DXVECTOR3 camPos, direction, modelPos(0, 0, 0), axis;
	values->MainCamera->Position(&camPos);

	direction = modelPos - camPos;
	D3DXVec3Normalize(&direction, &direction);

	D3DXVec3Cross(&axis, &D3DXVECTOR3(0, 0, 1), &direction);
	D3DXVec3Normalize(&axis, &axis);

	float angle = acos(D3DXVec3Dot(&D3DXVECTOR3(0, 0, 1), &direction));
	D3DXQuaternionRotationAxis(&quat, &axis, angle);

#endif

#endif
	m_sphere->Update();
	m_terrain->Update();
}

void ExeHW1::PreRender()
{
	m_sphere->PreRender();
	m_terrain->PreRender();
}

void ExeHW1::Render()
{
	m_sphere->Render();
	m_terrain->Render();
}

void ExeHW1::PostRender()
{
	char str[256];
	values->MainCamera->RotationDegree(&dgr);
	sprintf(str, "Degree: %f, %f, %f", dgr.x, dgr.y, degree);
	ImGui::Text(str);
	m_sphere->PostRender();
	m_terrain->PostRender();
}

void ExeHW1::ResizeScreen()
{
}

