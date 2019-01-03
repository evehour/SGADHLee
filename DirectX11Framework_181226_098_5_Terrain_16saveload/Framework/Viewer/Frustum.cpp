#include "Framework.h"
#include "Frustum.h"

#include "../Viewer/Camera.h"
#include "../Viewer/Perspective.h"

Frustum::Frustum(float zFar, Camera * camera, Perspective * perspective)
	: zFar(zFar), camera(camera), perspective(perspective)
{
	if (camera == NULL)
		this->camera = Context::Get()->GetMainCamera();

	if (perspective == NULL)
		this->perspective = Context::Get()->GetPerspective();
}

Frustum::~Frustum()
{

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


	D3DXMATRIX M;
	D3DXMatrixMultiply(&M, &V, &P);

	//Near
	planes[0].a = M._14 + M._13;
	planes[0].b = M._24 + M._23;
	planes[0].c = M._34 + M._33;
	planes[0].d = M._44 + M._43;

	//Far
	planes[1].a = M._14 - M._13;
	planes[1].b = M._24 - M._23;
	planes[1].c = M._34 - M._33;
	planes[1].d = M._44 - M._43;


	//Left
	planes[2].a = M._14 + M._11;
	planes[2].b = M._24 + M._21;
	planes[2].c = M._34 + M._31;
	planes[2].d = M._44 + M._41;

	//Right
	planes[3].a = M._14 - M._11;
	planes[3].b = M._24 - M._21;
	planes[3].c = M._34 - M._31;
	planes[3].d = M._44 - M._41;


	//Top
	planes[4].a = M._14 - M._12;
	planes[4].b = M._24 - M._22;
	planes[4].c = M._34 - M._32;
	planes[4].d = M._44 - M._42;

	//Bottom
	planes[5].a = M._14 + M._12;
	planes[5].b = M._24 + M._22;
	planes[5].c = M._34 + M._32;
	planes[5].d = M._44 + M._42;

	for (int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&planes[i], &planes[i]);
}

bool Frustum::ContainPoint(D3DXVECTOR3 & position)
{
	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &position) < 0.0f)
			return false;
	}

	return true;
}

bool Frustum::ContainRect(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	for (int i = 0; i< 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
			continue;

		return false;
	}

	return true;
}

bool Frustum::ContainRect(D3DXVECTOR3 center, D3DXVECTOR3 size)
{
	return ContainRect(center.x, center.y, center.z, size.x, size.y, size.z);
}

bool Frustum::ContainCube(D3DXVECTOR3 & center, float radius)
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

bool Frustum::ContainCircle(const D3DXVECTOR3 & center, const float radius)
{
	for (int i = 0; i < 6; i++)
	{
		//Ax+By+Cz+D=0
		if ((D3DXPlaneDotCoord(&planes[i], &center) / sqrtf(planes[i].a * planes[i].a + planes[i].b * planes[i].b + planes[i].c * planes[i].c)) <= radius)
			return true;
	}

	return false;
}

void Frustum::GetPlanes(OUT D3DXPLANE * plane)
{
	D3DXMATRIX V, P;
	camera->Matrix(&V);
	perspective->GetMatrix(&P);

	D3DXMATRIX vp;
	D3DXMatrixMultiply(&vp, &V, &P);

	//left
	plane[0] = D3DXPLANE(vp._14 + vp._11, vp._24 + vp._21, vp._34 + vp._31, vp._44 + vp._41);
	//right
	plane[1] = D3DXPLANE(vp._14 - vp._11, vp._24 - vp._21, vp._34 - vp._31, vp._44 - vp._41);
	//bottom
	plane[2] = D3DXPLANE(vp._14 + vp._12, vp._24 + vp._22, vp._34 + vp._32, vp._44 + vp._42);
	//top
	plane[3] = D3DXPLANE(vp._14 - vp._12, vp._24 - vp._22, vp._34 - vp._32, vp._44 - vp._42);
	//near
	plane[4] = D3DXPLANE(vp._13, vp._23, vp._33, vp._43);
	//far
	plane[5] = D3DXPLANE(vp._14 - vp._13, vp._24 - vp._23, vp._34 - vp._33, vp._44 - vp._43);
	for (UINT i = 0; i < 6; i++)
		D3DXPlaneNormalize(&plane[i], &plane[i]);
}
