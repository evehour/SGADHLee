#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer()
	: enable(true), visible(true)
	, position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
{
	material = new Material();

	D3DXMatrixIdentity(&world);
}

Renderer::~Renderer()
{
	SAFE_DELETE(material);
}

void Renderer::Enable(bool val)
{
	enable = val;

	for (auto temp : Enabled)
		temp(val);
}

bool Renderer::Enable()
{
	return enable;
}

void Renderer::Visible(bool val)
{
	visible = val;

	for (auto temp : Visibled)
		temp(val);
}

bool Renderer::Visible()
{
	return visible;
}


D3DXMATRIX Renderer::World()
{
	return world;
}

void Renderer::Position(D3DXVECTOR3 & vec)
{
	position = vec;

	UpdateWorld();
}

void Renderer::Position(float x, float y, float z)
{
	Position(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 Renderer::Position()
{
	return position;
}

void Renderer::Scale(D3DXVECTOR3 & vec)
{
	scale = vec;

	UpdateWorld();
}

void Renderer::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 Renderer::Scale()
{
	return scale;
}

void Renderer::Rotation(D3DXVECTOR3 & vec)
{
	rotation = vec;

	UpdateWorld();
}

void Renderer::Rotation(float x, float y, float z)
{
	Rotation(D3DXVECTOR3(x, y, z));
}

void Renderer::RotationDegree(D3DXVECTOR3 & vec)
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToRadian(vec.x);
	temp.y = Math::ToRadian(vec.y);
	temp.z = Math::ToRadian(vec.z);

	Rotation(temp);
}

void Renderer::RotationDegree(float x, float y, float z)
{
	RotationDegree(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 Renderer::Rotation()
{
	return rotation;
}

D3DXVECTOR3 Renderer::RotationDegree()
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	return temp;
}

D3DXVECTOR3 Renderer::Direction()
{
	return direction;
}

D3DXVECTOR3 Renderer::Up()
{
	return up;
}

D3DXVECTOR3 Renderer::Right()
{
	return right;
}

void Renderer::UpdateWorld()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

	direction = D3DXVECTOR3(world._31, world._32, world._33);
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);
}
