#include "stdafx.h"
#include "GameRender.h"
#include "../Interfaces/IBoundable.h"

GameRender::GameRender()
	: enable(true), visible(true)
	, position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
	, BoundObject(NULL)
{
	D3DXMatrixIdentity(&world);
}

GameRender::~GameRender()
{
	SAFE_DELETE(BoundObject);
}

void GameRender::Enable(bool val)
{
	enable = val;

	for (auto temp : Enabled)
		temp(val);
}

bool GameRender::Enable()
{
	return enable;
}

void GameRender::Visible(bool val)
{
	visible = val;

	for (auto temp : Visibled)
		temp(val);
}

bool GameRender::Visible()
{
	return visible;
}

D3DXMATRIX GameRender::World()
{
	return world;
}

void GameRender::Position(D3DXVECTOR3& vec)
{
	position = vec;

	UpdateWorld();
}

void GameRender::Position(float x, float y, float z)
{
	Position(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 GameRender::Position()
{
	return position;
}

void GameRender::Scale(D3DXVECTOR3 & vec)
{
	scale = vec;

	UpdateWorld();
}

void GameRender::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 GameRender::Scale()
{
	return scale;
}

void GameRender::Rotation(D3DXVECTOR3 & vec)
{
	rotation = vec;

	UpdateWorld();
}

void GameRender::Rotation(float x, float y, float z)
{
	Rotation(D3DXVECTOR3(x, y, z));
}

void GameRender::RotationDegree(D3DXVECTOR3 & vec)
{
	D3DXVECTOR3 temp;
	temp.x = Math::ToRadian(vec.x);
	temp.y = Math::ToRadian(vec.y);
	temp.z = Math::ToRadian(vec.z);

	Rotation(temp);
}

void GameRender::RotationDegree(float x, float y, float z)
{
	RotationDegree(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 GameRender::Rotation()
{
	return rotation;
}

D3DXVECTOR3 GameRender::RotationDegree()
{
	D3DXVECTOR3 temp;
	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	return temp;
}

D3DXVECTOR3 GameRender::Direction()
{
	return direction;
}

D3DXVECTOR3 GameRender::Up()
{
	return up;
}

D3DXVECTOR3 GameRender::Right()
{
	return right;
}

bool GameRender::Select(Ray * ray, float* distance)
{
	float dist = 0.0f;
	bool isIntersect = BoundObject->Intersect(ray, dist);

	if (distance != NULL) *distance = dist;

	return isIntersect;
}

void GameRender::UpdateWorld()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

#if true
	direction = D3DXVECTOR3(world._31, world._32, world._33);
	//direction *= -1.0f;//�⺻ ���� ���⿡ ���� ������ �ʿ���.
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);
#else
	//�⺻ ���� ���⿡ ���� ������ �ʿ��Ͽ���.
	D3DXVec3TransformNormal(&direction, &D3DXVECTOR3(0, 0, -1), &world);
	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &world);
	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &world);
#endif
}
