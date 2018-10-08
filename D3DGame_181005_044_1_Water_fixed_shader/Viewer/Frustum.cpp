#include "stdafx.h"
#include "Frustum.h"
#include "../Physics/LineMake.h"

Frustum::Frustum(ExecuteValues * values, float zFar, Camera * camera, Perspective * perspective)
	:values(values), zFar(zFar)
	, camera(camera), perspective(perspective)
{
	if (this->camera == NULL)
		this->camera = values->MainCamera;

	if (this->perspective == NULL)
		this->perspective = values->Perspective;

	lineMake = new LineMake();
}

Frustum::~Frustum()
{
	SAFE_DELETE(lineMake);
}

void Frustum::Update()
{
	D3DXMATRIX V, P;
	camera->Matrix(&V);
	perspective->GetMatrix(&P);
	
	float zNear = -P._43 / P._33;
	float r = zFar / (zFar - zNear);

	P._33 = r;
	P._43 = -r * zNear;

	D3DXMATRIX matrix;
	D3DXMatrixMultiply(&matrix, &V, &P);

	//Near
	planes[0].a = matrix._14 + matrix._13;
	planes[0].b = matrix._24 + matrix._23;
	planes[0].c = matrix._34 + matrix._33;
	planes[0].d = matrix._44 + matrix._43;

	//Far
	planes[1].a = matrix._14 - matrix._13;
	planes[1].b = matrix._24 - matrix._23;
	planes[1].c = matrix._34 - matrix._33;
	planes[1].d = matrix._44 - matrix._43;

	//Left
	planes[2].a = matrix._14 + matrix._11;
	planes[2].b = matrix._24 + matrix._21;
	planes[2].c = matrix._34 + matrix._31;
	planes[2].d = matrix._44 + matrix._41;

	//Right
	planes[3].a = matrix._14 - matrix._11;
	planes[3].b = matrix._24 - matrix._21;
	planes[3].c = matrix._34 - matrix._31;
	planes[3].d = matrix._44 - matrix._41;

	//Top
	planes[4].a = matrix._14 - matrix._12;
	planes[4].b = matrix._24 - matrix._22;
	planes[4].c = matrix._34 - matrix._32;
	planes[4].d = matrix._44 - matrix._42;

	//Bottom
	planes[5].a = matrix._14 + matrix._12;
	planes[5].b = matrix._24 + matrix._22;
	planes[5].c = matrix._34 + matrix._32;
	planes[5].d = matrix._44 + matrix._42;

	for (int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&planes[i], &planes[i]);

	// Using Multi-Picking
	pickAreaPlanes[0] = planes[0]; //Near
	pickAreaPlanes[1] = planes[1]; //Far

	lineMake->ClearBuffer();
	D3DXVECTOR3 origin, dir;
	IntersectPlanePlane(planes[0], planes[2], origin, dir);
	IntersectRayPlane(origin, dir, planes[4], &frustumArea[0]);
	IntersectRayPlane(origin, dir, planes[5], &frustumArea[3]);

	IntersectPlanePlane(planes[3], planes[0], origin, dir);
	IntersectRayPlane(origin, dir, planes[4], &frustumArea[1]);
	IntersectRayPlane(origin, dir, planes[5], &frustumArea[2]);

	IntersectPlanePlane(planes[2], planes[1], origin, dir);
	IntersectRayPlane(origin, dir, planes[4], &frustumArea[4]);
	IntersectRayPlane(origin, dir, planes[5], &frustumArea[7]);

	IntersectPlanePlane(planes[1], planes[3], origin, dir);
	IntersectRayPlane(origin, dir, planes[4], &frustumArea[5]);
	IntersectRayPlane(origin, dir, planes[5], &frustumArea[6]);

	lineMake->AddLine(frustumArea[0], frustumArea[1]);
	lineMake->AddLine(frustumArea[1], frustumArea[2]);
	lineMake->AddLine(frustumArea[2], frustumArea[3]);
	lineMake->AddLine(frustumArea[3], frustumArea[0]);

	lineMake->AddLine(frustumArea[4], frustumArea[5]);
	lineMake->AddLine(frustumArea[5], frustumArea[6]);
	lineMake->AddLine(frustumArea[6], frustumArea[7]);
	lineMake->AddLine(frustumArea[7], frustumArea[4]);

	lineMake->AddLine(frustumArea[0], frustumArea[4]);
	lineMake->AddLine(frustumArea[1], frustumArea[5]);
	lineMake->AddLine(frustumArea[2], frustumArea[6]);
	lineMake->AddLine(frustumArea[3], frustumArea[7]);

	lineMake->UpdateBuffer();
}

void Frustum::Render()
{
	lineMake->Render();
}

bool Frustum::ContainPoint(D3DXVECTOR3 & position, const int & cases)
{
	for (int i = 0; i < 6; i++)
	{
		switch (cases)
		{
		case 0:
			if (D3DXPlaneDotCoord(&planes[i], &position) < 0.0f)
				return false;
			break;
		case 1:
			if (D3DXPlaneDotCoord(&pickAreaPlanes[i], &position) < 0.0f)
				return false;
			break;
		}
	}
	return true;
}

bool Frustum::ContainCube(D3DXVECTOR3 & center, const float & radius, const int & cases)
{
	D3DXVECTOR3 check;
	for (int i = 0; i < 6; i++)
	{
		check.x = center.x - radius;
		check.y = center.y - radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y - radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x - radius;
		check.y = center.y + radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y + radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x - radius;
		check.y = center.y - radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y - radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x - radius;
		check.y = center.y + radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y + radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		return false;
	}
	return true;
}

D3DXVECTOR3 Frustum::IntersectRayPlane(const D3DXVECTOR3 & rayOrigin, const D3DXVECTOR3 & rayDirection, const D3DXPLANE & plane, OUT D3DXVECTOR3 * position)
{
	D3DXVECTOR3 _pos;
	// Ray-Plane Intersection
	// origin = Ray origin point, direction = Ray direction
	// P = Intersection Point, N = Plane Normal(a,b,c), D = Distance from 0,0,0 (d)
	//
	// t = -(dot(origin, N) + D) / dot(direction, N);
	// P = origin + (direction * t);
	float t = -(D3DXPlaneDotCoord(&plane, &rayOrigin)) / D3DXPlaneDotNormal(&plane, &rayDirection);
	_pos = rayOrigin + (rayDirection * t);

	if (position != NULL)
		*position = _pos;

	return _pos;
}

void Frustum::IntersectPlanePlane(const D3DXPLANE & p1, const D3DXPLANE & p2, OUT D3DXVECTOR3 & rayOrigin, OUT D3DXVECTOR3 & rayDirection)
{
	D3DXVECTOR3 p1N(p1.a, p1.b, p1.c), p2N(p2.a, p2.b, p2.c);
	D3DXVec3Cross(&rayDirection, &p1N, &p2N);
	float num = D3DXVec3LengthSq(&rayDirection);

	D3DXVECTOR3 p1p2Cross = (-p1.d * p2N) + (p2.d * p1N);

	D3DXVec3Cross(&rayOrigin,
		&p1p2Cross,
		&rayDirection);

	rayOrigin = rayOrigin / num;

}

void Frustum::SetPickingArea(const D3DXVECTOR3 & start, const D3DXVECTOR3 & end, D3DXVECTOR3* const arr)
{
	D3DXVECTOR3 point[2];
	D3DXVECTOR3 cameraPos;
	D3DXVECTOR3 mouseRectPos[4];
	D3DXVECTOR3 directionOriginToMRP[4];
	D3DXVECTOR3 intersectPointInPlane[4];

	point[0].x = start.x < end.x ? start.x : end.x;
	point[0].y = start.y < end.y ? start.y : end.y;
	point[0].z = start.z < end.z ? start.z : end.z;

	point[1].x = start.x > end.x ? start.x : end.x;
	point[1].y = start.y > end.y ? start.y : end.y;
	point[1].z = start.z > end.z ? start.z : end.z;

	camera->Position(&cameraPos);

	// Making 2D area from mouse lbutton Down/Up position
	mouseRectPos[0] = D3DXVECTOR3(point[0].x, point[0].y, 0); // LT
	mouseRectPos[1] = D3DXVECTOR3(point[1].x, point[0].y, 0); // RT
	mouseRectPos[2] = D3DXVECTOR3(point[1].x, point[1].y, 0); // RB
	mouseRectPos[3] = D3DXVECTOR3(point[0].x, point[1].y, 0); // LB

	for (int i = 0; i < 4; i++)
	{
		directionOriginToMRP[i] = camera->Direction(values->Viewport, perspective, mouseRectPos[i]);
		D3DXVec3Normalize(&directionOriginToMRP[i], &directionOriginToMRP[i]);

		IntersectRayPlane(cameraPos, directionOriginToMRP[i], pickAreaPlanes[1], &intersectPointInPlane[i]);
	}

	// Making frustum area using plane.
	D3DXPlaneFromPoints(&pickAreaPlanes[2], &cameraPos, &intersectPointInPlane[1], &intersectPointInPlane[0]); // Top
	D3DXPlaneFromPoints(&pickAreaPlanes[4], &cameraPos, &intersectPointInPlane[3], &intersectPointInPlane[2]); // Bottom
	D3DXPlaneFromPoints(&pickAreaPlanes[5], &cameraPos, &intersectPointInPlane[0], &intersectPointInPlane[3]); // Left
	D3DXPlaneFromPoints(&pickAreaPlanes[3], &cameraPos, &intersectPointInPlane[2], &intersectPointInPlane[1]); // Right

	if (arr != NULL)
	{
		for (int i = 0; i < 4; i++)
			arr[i] = intersectPointInPlane[i];
	}
}

void Frustum::GetFrustumArea(D3DXVECTOR3 * arr)
{
	memcpy((void*)arr, (void*)frustumArea, sizeof(D3DXVECTOR3) * 8);
}
