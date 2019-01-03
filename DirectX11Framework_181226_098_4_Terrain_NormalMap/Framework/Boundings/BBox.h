#pragma once
#include "Framework.h"
#include "../Draw/ILine.h"
#include "../Interfaces/IBoundable.h"

struct BBox : public ILine, public IBoundable
{
private:
	struct OBB
	{
		D3DXVECTOR3 Center;
		D3DXVECTOR3 AxisX;
		D3DXVECTOR3 AxisY;
		D3DXVECTOR3 AxisZ;
		D3DXVECTOR3 HalfSize;

		OBB()
			: Center(0, 0, 0)
			, AxisX(1, 0, 0), AxisY(0, 1, 0), AxisZ(0, 0, 1)
			, HalfSize(0.5f, 0.5f, 0.5f)
		{}
	};

public:
	BBox()
	{
		Min = Max = D3DXVECTOR3(0, 0, 0);
	}
	
	BBox(D3DXVECTOR3& min, D3DXVECTOR3& max)
		: Min(min), Max(max)
	{
		
	}

	bool Intersect(BBox* target, const D3DXMATRIX& selfWorld, const D3DXMATRIX& targetWorld)
	{
		OBB selfOBB, targetOBB;
		selfOBB.Center = (Max - Min) / 2.0f;
		selfOBB.HalfSize = Max - selfOBB.Center;

		D3DXVec3TransformCoord(&selfOBB.Center, &selfOBB.Center, &selfWorld);
		D3DXVec3TransformCoord(&selfOBB.HalfSize, &selfOBB.HalfSize, &selfWorld);
		
		D3DXVec3TransformCoord(&targetOBB.Center, &targetOBB.Center, &targetWorld);
		D3DXVec3TransformCoord(&targetOBB.HalfSize, &targetOBB.HalfSize, &targetWorld);

		D3DXVECTOR3 RPos = targetOBB.Center - selfOBB.Center;

		bool bnIntersect = false;

		bnIntersect |= GetSeparateAxis(RPos, selfOBB.AxisX, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, selfOBB.AxisY, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, selfOBB.AxisZ, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, targetOBB.AxisX, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, targetOBB.AxisY, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, targetOBB.AxisZ, selfOBB, targetOBB);

		D3DXVECTOR3 _nAxisX, _nAxisY, _nAxisZ;

		D3DXVec3Cross(&_nAxisX, &selfOBB.AxisX, &targetOBB.AxisX);
		D3DXVec3Cross(&_nAxisY, &selfOBB.AxisX, &targetOBB.AxisY);
		D3DXVec3Cross(&_nAxisZ, &selfOBB.AxisX, &targetOBB.AxisZ);
		bnIntersect |= GetSeparateAxis(RPos, _nAxisX, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, _nAxisY, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, _nAxisZ, selfOBB, targetOBB);

		D3DXVec3Cross(&_nAxisX, &selfOBB.AxisY, &targetOBB.AxisX);
		D3DXVec3Cross(&_nAxisY, &selfOBB.AxisY, &targetOBB.AxisY);
		D3DXVec3Cross(&_nAxisZ, &selfOBB.AxisY, &targetOBB.AxisZ);
		bnIntersect |= GetSeparateAxis(RPos, _nAxisX, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, _nAxisY, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, _nAxisZ, selfOBB, targetOBB);

		D3DXVec3Cross(&_nAxisX, &selfOBB.AxisZ, &targetOBB.AxisX);
		D3DXVec3Cross(&_nAxisY, &selfOBB.AxisZ, &targetOBB.AxisY);
		D3DXVec3Cross(&_nAxisZ, &selfOBB.AxisZ, &targetOBB.AxisZ);
		bnIntersect |= GetSeparateAxis(RPos, _nAxisX, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, _nAxisY, selfOBB, targetOBB);
		bnIntersect |= GetSeparateAxis(RPos, _nAxisZ, selfOBB, targetOBB);

		return !bnIntersect;
	}

	bool Intersect(Ray* ray, OUT float& result)
	{
		result = 0.0f;
		float minValue = 0.0f, maxValue = FLT_MAX;

		//Check X
		if (fabsf(ray->Direction.x) >= 1e-6f)
		{
			float value = 1.0f / ray->Direction.x;
			float minX = (Min.x - ray->Position.x) * value;
			float maxX = (Max.x - ray->Position.x) * value;

			if (minX > maxX)
			{
				float temp = minX;
				minX = maxX;
				maxX = temp;
			}

			minValue = max(minX, minValue);
			maxValue = min(maxX, maxValue);

			if (minValue > maxValue)
				return false;
		}
		else if (ray->Position.x < Min.x || ray->Position.x > Max.x)
			return false;

		//Check Y
		if (fabsf(ray->Direction.y) >= 1e-6f)
		{
			float value = 1.0f / ray->Direction.y;
			float minY = (Min.y - ray->Position.y) * value;
			float maxY = (Max.y - ray->Position.y) * value;

			if (minY > maxY)
			{
				float temp = minY;
				minY = maxY;
				maxY = temp;
			}

			minValue = max(minY, minValue);
			maxValue = min(maxY, maxValue);

			if (minValue > maxValue)
				return false;
		}
		else if (ray->Position.y < Min.y || ray->Position.y > Max.y)
			return false;


		//Check Z
		if (fabsf(ray->Direction.z) >= 1e-6f)
		{
			float value = 1.0f / ray->Direction.z;
			float minZ = (Min.z - ray->Position.z) * value;
			float maxZ = (Max.z - ray->Position.z) * value;

			if (minZ > maxZ)
			{
				float temp = minZ;
				minZ = maxZ;
				maxZ = temp;
			}

			minValue = max(minZ, minValue);
			maxValue = min(maxZ, maxValue);

			if (minValue > maxValue)
				return false;
		}
		else if (ray->Position.z < Min.z || ray->Position.z > Max.z)
			return false;

		result = minValue;
		return true;
	}

	void GetLine(D3DXMATRIX& world, vector<D3DXVECTOR3>& lines)
	{
		D3DXVECTOR3 tempMin;
		D3DXVECTOR3 tempMax;

		D3DXVec3TransformCoord(&tempMin, &Min, &world);
		D3DXVec3TransformCoord(&tempMax, &Max, &world);


		//Front
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMin.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMin.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMax.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMax.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMin.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMax.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMin.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMax.y, tempMin.z));

		//Back
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMin.y, tempMax.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMin.y, tempMax.z));
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMax.y, tempMax.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMax.y, tempMax.z));
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMin.y, tempMax.z));
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMax.y, tempMax.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMin.y, tempMax.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMax.y, tempMax.z));

		//Middle
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMin.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMin.y, tempMax.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMin.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMin.y, tempMax.z));
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMax.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMin.x, tempMax.y, tempMax.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMax.y, tempMin.z));
		lines.push_back(D3DXVECTOR3(tempMax.x, tempMax.y, tempMax.z));
	}

public:
	D3DXVECTOR3 Min;
	D3DXVECTOR3 Max;

private:
	// check if there's a separating normal axis in between the selected axes
	bool GetSeparateAxis(const D3DXVECTOR3& RPos, const D3DXVECTOR3& nAxis, const OBB& box1, const OBB& box2)
	{
		float d = fabs(D3DXVec3Dot(&RPos, &nAxis));
		float dd = 0.0f;

		dd += fabs(D3DXVec3Dot(&(box1.AxisX * box1.HalfSize.x), &nAxis));
		dd += fabs(D3DXVec3Dot(&(box1.AxisY * box1.HalfSize.y), &nAxis));
		dd += fabs(D3DXVec3Dot(&(box1.AxisZ * box1.HalfSize.z), &nAxis));
		dd += fabs(D3DXVec3Dot(&(box2.AxisX * box2.HalfSize.x), &nAxis));
		dd += fabs(D3DXVec3Dot(&(box2.AxisY * box2.HalfSize.y), &nAxis));
		dd += fabs(D3DXVec3Dot(&(box2.AxisZ * box2.HalfSize.z), &nAxis));

		return d > dd;
	}
};