#pragma once
#include "stdafx.h"
#include "../Draw/ILine.h"

struct Ray : public ILine
{
	Ray()
	{
		Position = Direction = D3DXVECTOR3(0, 0, 0);
	}

	Ray(D3DXVECTOR3& position, D3DXVECTOR3& direction)
		: Position(position), Direction(direction)
	{

	}

	void SettingRay(const D3DXVECTOR3& position, const D3DXVECTOR3& direction, const D3DXMATRIX& targetMatrix, const float distance = 1000.0f)
	{
		D3DXMATRIX invWorld;
		D3DXMatrixInverse(&invWorld, NULL, &targetMatrix);

		D3DXVec3TransformCoord(&Position, &position, &invWorld);
		D3DXVec3TransformNormal(&Direction, &direction, &invWorld);
		D3DXVec3Normalize(&Direction, &Direction);

		Distance = distance;
	}

	void GetLine(D3DXMATRIX& world, vector<D3DXVECTOR3>& lines)
	{
		lines.push_back(Position);
		lines.push_back(Position + (Direction * Distance));
	}

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Direction;
	float Distance = 10.0f;
};