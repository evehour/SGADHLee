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

#if 1
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

		dXZ.x = direction.x;
		dXZ.z = direction.z;

		D3DXVec3Normalize(&dXY, &dXY);
		D3DXVec3Normalize(&dXZ, &dXZ);
		D3DXVec3Normalize(&direction, &direction);

		// X°¢µµ
		double xyCos = acos((double)D3DXVec3Dot(&dXY, &D3DXVECTOR3(1, 0, 0)));
		double xzCos = acos((double)D3DXVec3Dot(&dXZ, &D3DXVECTOR3(0, 0, 1)));

		if (direction.y >= 0.0f)
			xyCos = -xyCos;
		if (direction.x < 0.0f)
			xzCos = -xzCos;

		// rad to deg
		float degxyCos;
		float degxzCos;
		degxyCos = xyCos * 180.0f / D3DX_PI;
		degxzCos = xzCos * 180.0f / D3DX_PI;
		values->MainCamera->Rotation(xyCos, xzCos);
	}
#else
	D3DXVECTOR3 camPos;
	values->MainCamera->Position(&camPos);
	//if(beforeCamPos != camPos)
	{
		float degxyCos;
		float degxzCos;
		D3DXVECTOR3 direction, modelPos(0, 0, 0), fwd, right;
		D3DXVECTOR2 camRot;
		D3DXVECTOR3 dDot, dXY(0, 0, 0), dXZ(0, 0, 0);


		direction = modelPos - camPos;
		
		dXY = dXZ = direction;
		dXY.x = sqrt(direction.x * direction.x + direction.z * direction.z);
		dXY.z = 0;
		dXZ.y = 0;
		D3DXVec3Normalize(&dXY, &dXY);
		D3DXVec3Normalize(&dXZ, &dXZ);
		D3DXVec3Normalize(&direction, &direction);
		
		values->MainCamera->Rotation(&camRot);
		values->MainCamera->Right(&right);
		right = D3DXVECTOR3(1, 0, 0);
		double xyCos = acos((double)D3DXVec3Dot(&right, &dXY));
		degxyCos = xyCos * 180.0f / D3DX_PI;
		values->MainCamera->Rotation((float)xyCos, camRot.y);
		
		values->MainCamera->Rotation(&camRot);
		values->MainCamera->Forward(&fwd);
		fwd = D3DXVECTOR3(0, 0, 1);
		float xzCos = acos(D3DXVec3Dot(&dXZ, &fwd));
		if (direction.x < 0.0f)
			xzCos = -xzCos;
		degxzCos = xzCos * 180.0f / D3DX_PI;
		values->MainCamera->Rotation(camRot.x, xzCos);
		values->MainCamera->Forward(&fwd);
		beforeCamPos = camPos;
	}
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

