#include "stdafx.h"
#include "Environment/PathFinding.h"
#include "Environment/Terrain.h"

#include "Unit.h"

const float Unit::HeightOffset = 0.1f;

Unit::Unit(MapTile * mapTile, PathFinding * pathFinding)
	: mapTile(mapTile), pathFinding(pathFinding)
{
	position = mapTile->WorldPosition;
	position.y += HeightOffset;

	bMoving = false;
	movePrc = 0.0f;
	time = 0.0f;

	speed = 10.0f;

	material = new Material(Shaders + L"052_Mesh.fx");
	sphere = new MeshSphere(material, 1.0f);

	D3DXVECTOR3 scale;
	sphere->Scale(scale);
	// + scale.y * 0.5f
	sphere->Position(position);
}

Unit::~Unit()
{
	SAFE_DELETE(sphere);
	SAFE_DELETE(material);
}

void Unit::Update()
{
	time += speed * Time::Delta();

	if (bMoving)
	{
		if (movePrc < 1.0f)
			movePrc += Time::Delta() * speed;

		if (movePrc > 1.0f)
			movePrc = 1.0f;

		if (fabsf(movePrc - 1.0f) < Math::EPSILON)
		{
			if (activeWP + 1 >= pathes.size())
				bMoving = false;
			else
			{
				activeWP++;
				MoveUnit(pathes[activeWP]);
			}
		}

		D3DXVec3Lerp(&position, &lastWP, &nextWP, movePrc);
	}

	sphere->Position(position);
}

void Unit::Render()
{
	sphere->Render();
}

void Unit::Goto(MapTile * tile)
{
	pathes.clear();
}

void Unit::MoveUnit(MapTile * to)
{
	lastWP = mapTile->WorldPosition;
	lastWP.y = pathFinding->GetTerrain()->GetHeight(lastWP.x, lastWP.z) + HeightOffset;;

	mapTile = to;
	movePrc = 0.0f;

	nextWP = mapTile->WorldPosition;
	nextWP.y = pathFinding->GetTerrain()->GetHeight(nextWP.x, nextWP.z) + HeightOffset;
}
