#include "stdafx.h"
#include "Collision.h"
#include "Objects/GameModel.h"

bool Collision::ShapeCollisionDetectAuto(GameModel & gmA, GameModel & gmB)
{
	bool intersect = false;
	if (gmA.BoundType() == GameModel::BOUND_TYPE_BOX)
	{
		switch (gmB.BoundType())
		{
		case GameModel::BOUND_TYPE_BOX:
			intersect = IntersectRectToRect(gmA, gmB);
			break;
		case GameModel::BOUND_TYPE_SPHERE:
			break;
		}
	}
	else if (gmA.BoundType() == GameModel::BOUND_TYPE_SPHERE)
	{
		switch (gmB.BoundType())
		{
		case GameModel::BOUND_TYPE_BOX:
			break;
		case GameModel::BOUND_TYPE_SPHERE:
			break;
		}
	}
	return false;
}

bool Collision::IntersectRectToRect(GameModel& gmA, GameModel& gmB)
{
	std::vector<D3DXVECTOR3> pointsA, pointsB;
	D3DXVECTOR3 posA = gmA.Position(), posB = gmB.Position();
	D3DXVECTOR3 centerA, centerB, proj;
	float radiusA, radiusB, clA, clB;

	gmA.Center(centerA);
	gmB.Center(centerB);
	posA += centerA;
	posB += centerB;

	// 1차 검증
	gmA.Radius(radiusA);
	gmB.Radius(radiusB);
	proj = posB - posA;
	if (D3DXVec3Length(&proj) > (radiusA + radiusB)) return false;
	
	// 2차 검증
	bool b = true;
	gmA.GetAAABB(pointsA);
	gmB.GetAAABB(pointsB);

	b &= (pointsA[0].x <= pointsB[7].x) && (pointsA[7].x >= pointsB[0].x);
	b &= (pointsA[0].y <= pointsB[7].y) && (pointsA[7].y >= pointsB[0].y);
	b &= (pointsA[0].z <= pointsB[7].z) && (pointsA[7].z >= pointsB[0].z);

	if (b) return true;

	return false;
}

bool Collision::IntersectCircleToCircle(GameModel & gmA, GameModel & gmB)
{
	D3DXVECTOR3 proj = gmA.Position() - gmB.Position();
	float length = D3DXVec3Length(&proj);
	float radiusA, radiusB;

	gmA.Radius(radiusA);
	gmB.Radius(radiusB);

	if (length < radiusA + radiusB)
		return true;
	
	return false;
}
