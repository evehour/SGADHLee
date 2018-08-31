#include "stdafx.h"
#include "Frustum.h"

Frustum::Frustum(ExecuteValues * values, float zFar, Camera * camera, Perspective * perspective)
	:values(values), zFar(zFar)
	, camera(camera), perspective(perspective)
	, vMin(-1, -1, -1), vMax(1, 1, 1)
{
	if (this->camera == NULL)
		this->camera = values->MainCamera;

	if (this->perspective == NULL)
		this->perspective = values->Perspective;

	D3DXPLANE a, b;
	D3DXVECTOR3 k1(0, 1, 0);
	D3DXVECTOR3 k2(0, 0, 0);
	D3DXVECTOR3 k3(1, 0, 0);
	D3DXVECTOR3 j1(-1,  1, -1);
	D3DXVECTOR3 j2(-1, -1, -1);
	D3DXVECTOR3 j3( 1, -1, -1);
	D3DXPlaneFromPoints(&a, &k2, &k3, &k1);
	D3DXPlaneFromPoints(&b, &j2, &j3, &j1);

	a = a;
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

void Frustum::CreateData()
{
	D3DXMATRIX V, P, M;
	camera->Matrix(&V);
	perspective->GetMatrix(&P);
	D3DXMatrixMultiply(&M, &V, &P);

}
