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

bool Collision::IntersectRaySphere(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir, D3DXVECTOR3 sphereOrigin, float sphereRadius)
{
	D3DXVECTOR3 VRS = rayOrigin - sphereOrigin;

	float a = D3DXVec3Dot(&rayDir, &rayDir);
	float b = 2.0f * D3DXVec3Dot(&VRS, &rayDir);
	float c = D3DXVec3Dot(&VRS, &VRS) - (sphereRadius * sphereRadius);

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return false;

	//float tmin = (-b - sqrtf(discriminant)) / (2.0f * a);
	//float tmax = (-b + sqrtf(discriminant)) / (2.0f * a);
	//if (tmin > tmax)
	//{
	//	float temp = tmin;
	//	tmin = tmax;
	//	tmax = temp;
	//}

	return true;
}

bool Collision::IntersectRectToRect(GameModel& gmA, GameModel& gmB)
{
	std::vector<D3DXVECTOR3> pointsA, pointsB;
	D3DXVECTOR3 posA = gmA.Position(), posB = gmB.Position();
	D3DXVECTOR3 centerA, centerB, proj;
	float radiusA, radiusB;

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

float Collision::ClosestPtSegmentSegment(const D3DXVECTOR3 & p1, const D3DXVECTOR3 & q1, const D3DXVECTOR3 & p2, const D3DXVECTOR3 & q2, float & s, float & t, D3DXVECTOR3 & c1, D3DXVECTOR3 & c2)
{
	D3DXVECTOR3 d1 = q1 - p1;
	D3DXVECTOR3 d2 = q2 - p2;
	D3DXVECTOR3 r = p1 - p2;

	float a = D3DXVec3Dot(&d1, &d1);
	float e = D3DXVec3Dot(&d2, &d2);
	float f = D3DXVec3Dot(&d2, &r);

	if (a <= Math::EPSILON && e <= Math::EPSILON)
	{
		s = t = 0.0f;
		c1 = p1;
		c2 = p2;

		return D3DXVec3Dot(&(c1 - c2), &(c1 - c2));
	}

	if (a <= Math::EPSILON)
	{
		s = 0.0f;
		t = f / e; // s = 0 => t = (b*s + f) / e = f / e
		t = Math::Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = D3DXVec3Dot(&d1, &r);
		if (e <= Math::EPSILON)
		{
			t = 0.0f;
			s = Math::Clamp(-c / a, 0.0f, 1.0f);
		}
		else
		{
			float b = D3DXVec3Dot(&d1, &d2);
			float denom = a*e - b*b; // 무조건 양수로 나옴

			if (denom != 0.0f)
			{
				s = Math::Clamp((b*f - c*e) / denom, 0.0f, 1.0f);
			}
			else
			{
				s = 0.0f;
			}

			t = (b*s + f) / e;

			if (t < 0.0f)
			{
				t = 0.0f;
				s = Math::Clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f)
			{
				t = 1.0f;
				s = Math::Clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;

	return D3DXVec3Dot(&(c1 - c2), &(c1 - c2));
}
