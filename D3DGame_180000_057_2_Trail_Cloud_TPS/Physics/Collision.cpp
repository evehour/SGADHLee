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

bool Collision::IntersectSphereToRay(const D3DXVECTOR3 & spherePosition, const float sphereRadius, const D3DXVECTOR3 & rayOrigin, const D3DXVECTOR3 & rayDirection, float * intersectionFD, float * intersectionSD)
{
#if false
	bool isIntersection = false;
	isIntersection = D3DXSphereBoundProbe(&spherePosition, sphereRadius, &rayOrigin, &rayDirection);

	return isIntersection;
#else
	float i1, i2;
	D3DXVECTOR3 rTs = rayOrigin - spherePosition;

	float dot = -D3DXVec3Dot(&rTs, &rayDirection);
	float det = (dot * dot) - D3DXVec3Dot(&rTs, &rTs) + (sphereRadius * sphereRadius);

	if (det < D3DX_16F_EPSILON)
	{
		return false;
	}

	det = sqrtf(det);
	i1 = dot - det;
	i2 = dot + det;

	if (i2 < 0) return false;
	if (i1 < 0) i1 = 0;

	if (intersectionFD != NULL) *intersectionFD = i1;
	if (intersectionSD != NULL) *intersectionSD = i2;

	return true;
#endif
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

float Collision::distSegmentToSegment(const D3DXVECTOR3 & s1, const D3DXVECTOR3 & s2, const D3DXVECTOR3 & k1, const D3DXVECTOR3 & k2)
{
	D3DXVECTOR3 u = s2 - s1;
	D3DXVECTOR3 v = k2 - k1;
	D3DXVECTOR3 w = s1 - k1;
	float a = D3DXVec3Dot(&u, &u);
	float b = D3DXVec3Dot(&u, &v);
	float c = D3DXVec3Dot(&v, &v);
	float d = D3DXVec3Dot(&u, &w);
	float e = D3DXVec3Dot(&v, &w);

	float D = a * c - b * b;
	float sc, sN, sD = D;
	float tc, tN, tD = D;

	if (D < D3DX_16F_EPSILON)
	{
		sN = 0.0f;
		sD = 1.0f;
		tN = e;
		tD = c;
	}
	else
	{
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0f)
		{
			sN = 0.0f;
			tN = e;
			tD = c;
		}
		else if (sN > sD)
		{
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0f)
	{
		tN = 0.0f;

		if (-d < 0.0f)
			sN = 0.0f;
		else if (-d > a)
			sN = sD;
		else
		{
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD)
	{
		tN = tD;

		if ((-d + b) < 0.0f)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else
		{
			sN = (-d + b);
			sD = a;
		}
	}

	sc = (fabs(sN) < D3DX_16F_EPSILON ? 0.0f : sN / sD);
	tc = (fabs(tN) < D3DX_16F_EPSILON ? 0.0f : tN / tD);

	D3DXVECTOR3 dP = w + (u * sc) - (v * tc);
	
	float dPDot;
	{
		dPDot = D3DXVec3Dot(&dP, &dP);
		dPDot = sqrtf(dPDot);
	} // == D3DXVec3Legnth()
	return dPDot;
}
