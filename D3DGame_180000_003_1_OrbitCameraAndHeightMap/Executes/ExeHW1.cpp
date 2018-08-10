#include "stdafx.h"
#include "./Executes/Sphere.h"
#include "./Executes/Terrain.h"
#include "ExeHW1.h"

ExeHW1::ExeHW1(ExecuteValues * values)
	: Execute(values)
	, degree(0), radius(5)
	, dgr(0,0)
	, beforeCamPos(0,0,0)
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
	D3DXVECTOR3 _pos{ 0,0,0 }, _pos2, _pos3;
	_pos3 = _pos2 = _pos;
	degree -= (20.0f * Time::Get()->Delta());

	if (degree > 360.0f)
		degree -= 360.0f;

	_pos.x = cosf(degree) * radius;
	_pos.y = sinf(degree) * radius;
	_pos.z = 0.0;

	_pos2.x = cosf(Math::ToRadian(degree)) * radius;
	_pos2.y = 0.0f;
	_pos2.z = sinf(Math::ToRadian(degree)) * radius;

	_pos3.x = 0.0f;
	_pos3.y = sinf(degree) * radius;
	_pos3.z = cosf(degree) * radius;

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

#if 1
	{
		float dgrSpeed = 10.0f;

		if (Keyboard::Get()->Press('A'))
			dgr.x -= 1.0f * dgrSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			dgr.x += 1.0f * dgrSpeed * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			dgr.y += 1.0f * dgrSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			dgr.y -= 1.0f * dgrSpeed * Time::Delta();
		//          x축으로 45도
		// 0,0,-5  --------------> ? , ? , ?
		//          y축으로 30도

		float r = 5.0f;
		D3DXVECTOR3 dPos;
		float x, y, z;
		float dX, dY;
		values->MainCamera->Position(&dPos);

		dX = Math::ToRadian(dgr.x);
		dY = Math::ToRadian(dgr.y);

		dPos.x = cosf(dX) * r;
		dPos.z = sinf(dY) * r;
		dPos.y = r / tanf(0.57);

		values->MainCamera->Position(dPos.x, dPos.y, -dPos.z);
	}
	{
		D3DXMATRIX rotX, rotY, rotXY, matView;
		D3DXVECTOR3 camPos, direction, modelPos(0, 0, 0), up, fwd, rot(0, 0, 0);
		D3DXVECTOR3 dXY(0, 0, 0), dXZ(0, 0, 0);
		D3DXVECTOR2 camRotation;
		values->MainCamera->Position(&camPos);
		values->MainCamera->Up(&up);
		values->MainCamera->Forward(&fwd);
		values->MainCamera->Rotation(&camRotation);
		values->MainCamera->Matrix(&matView);

		direction = modelPos - camPos;

		//dXY.x = direction.x;
		dXY.x = sqrt(direction.x * direction.x + direction.z * direction.z);
		dXY.y = direction.y;

		dXZ.x = direction.x; // 0.0f
		dXZ.y = 0.0f;
		dXZ.z = direction.z; // 5.0f

		D3DXVec3Normalize(&dXY, &dXY);
		D3DXVec3Normalize(&dXZ, &dXZ);
		D3DXVec3Normalize(&direction, &direction);

		// X각도
		double xyCos = acos((double)D3DXVec3Dot(&dXY, &D3DXVECTOR3(1, 0, 0)));
		double xzCos = acos((double)D3DXVec3Dot(&dXZ, &D3DXVECTOR3(0, 0, 1)));

		if (direction.y >= 0.0f)
			xyCos = -xyCos;
		if (direction.x <= 0.0f)
			xzCos = -xzCos;

		// rad to deg
		float degxyCos;
		float degxzCos;
		degxyCos = xyCos * 180.0f / D3DX_PI;
		degxzCos = xzCos * 180.0f / D3DX_PI;
		values->MainCamera->Rotation(xyCos, xzCos);
	}
#else
	
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
	/*values->MainCamera->RotationDegree(&dgr);
	sprintf(str, "Degree: %f, %f, %f", dgr.x, dgr.y, degree);
	ImGui::Text(str);*/

	D3DXVECTOR3 pos;
	values->MainCamera->Position(&pos);
	sprintf(str, "Position: %f, %f, %f", pos.x, pos.y, pos.z);
	ImGui::Text(str);

	m_sphere->PostRender();
	m_terrain->PostRender();
}

void ExeHW1::ResizeScreen()
{
}

