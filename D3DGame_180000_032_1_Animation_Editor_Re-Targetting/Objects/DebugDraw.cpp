#include "stdafx.h"
#include "DebugDraw.h"
#include "../Physics/LineMake.h"

DebugDraw::DebugDraw(DRAW_OBJECT_TYPE objNum)
	: GameModel(Materials + L"Meshes/", L"Cube.material", Models + L"Meshes/", L"Cube.mesh")
	, objNum(objNum)
{
	line = new LineMake();
	SettingObj();

	ChangeObj(objNum);
}

DebugDraw::~DebugDraw()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(line);
}

void DebugDraw::Update()
{
	__super::Update();
	line->SetWorld(World());
}

void DebugDraw::PreRender()
{
	
}

void DebugDraw::Render()
{
	line->Render();
}

void DebugDraw::PostRender()
{

}

void DebugDraw::ResizeScreen()
{
}

void DebugDraw::ChangeObj(DRAW_OBJECT_TYPE objNum)
{
	switch (objNum)
	{
	default:
	case DRAW_OBJECT_TYPE_SPHERE:
		vTargetVertices = &vSphereVertices;
		vTargetIndices = &vSphereIndices;
		break;
	case DRAW_OBJECT_TYPE_CUBE:
		vTargetVertices = &vCubeVertices;
		vTargetIndices = &vCubeIndices;
		break;
	case DRAW_OBJECT_TYPE_CAPSULE:
		vTargetVertices = &vCapsuleVertices;
		vTargetIndices = &vCapsuleIndices;
		break;
	}

	vector<D3DXVECTOR3> _vTmpVertices;
	for (UINT i = 0; i < vTargetVertices->size(); i++)
		_vTmpVertices.push_back((*vTargetVertices)[i].Position);

	line->MeshData(_vTmpVertices, *vTargetIndices, objNum);
	line->SetColor(D3DXCOLOR(1, 1, 0, 1));
}

D3DXCOLOR DebugDraw::GetColor()
{
	return line->GetColor();
}

void DebugDraw::SetColor(D3DXCOLOR & color)
{
	line->SetColor(color);
}

void DebugDraw::SettingObj()
{
	SettingObjCube();
	SettingObjSphere();
	SettingObjCapsule();
}

void DebugDraw::SettingObjCube()
{
#if true
	vector<D3DXVECTOR3> tempPos;
	for (UINT i = 0; i < 2; i++)
	{
		for (UINT j = 0; j < 4; j++)
		{
			D3DXVECTOR3 tmp;
			tmp.x = (j < 2) ? -1.0f : 1.0f;
			tmp.y = (j % 2 == 0) ? -1.0f : 1.0f;
			tmp.z = (i < 1) ? -1.0f : 1.0f;
			tempPos.push_back(tmp);
		}
	}

	for (UINT i = 0; i < tempPos.size(); i++)
	{
		VertexTextureNormal temp;
		temp.Position = tempPos[i];
		vCubeVertices.push_back(temp);
	}

	vCubeIndices.assign(36, 0);
	vCubeIndices = {
		0,1,2, 3,2,1,
		2,3,6, 7,6,3,
		6,7,4, 5,4,7,
		4,5,0, 1,0,5,
		1,5,3, 7,3,5,
		4,0,6, 2,6,0
	};
#else
	float radius = 1.0f;
	vector<D3DXVECTOR3> tempPos;
	tempPos.push_back(D3DXVECTOR3(-1, 1, -1) * radius);
	tempPos.push_back(D3DXVECTOR3(-1, -1, -1) * radius);
	tempPos.push_back(D3DXVECTOR3(1, 1, -1) * radius);
	tempPos.push_back(D3DXVECTOR3(1, -1, -1) * radius);

	tempPos.push_back(D3DXVECTOR3(-1, -1, 1) * radius);
	tempPos.push_back(D3DXVECTOR3(1, -1, 1) * radius);
	tempPos.push_back(D3DXVECTOR3(-1, 1, 1) * radius);
	tempPos.push_back(D3DXVECTOR3(1, 1, 1) * radius);

	tempPos.push_back(D3DXVECTOR3(-1, 1, -1) * radius);
	tempPos.push_back(D3DXVECTOR3(1, 1, -1) * radius);

	tempPos.push_back(D3DXVECTOR3(-1, 1, -1) * radius);
	tempPos.push_back(D3DXVECTOR3(-1, 1, 1) * radius);

	tempPos.push_back(D3DXVECTOR3(1, 1, -1) * radius);
	tempPos.push_back(D3DXVECTOR3(1, 1, 1) * radius);

	vector<D3DXVECTOR3> tempNorm;
	for (UINT i = 0; i < tempPos.size(); i++)
	{
		D3DXVECTOR3 temp;
		D3DXVec3Normalize(&temp, &tempPos[i]);
		tempNorm.push_back(temp);
	}

	vector<D3DXVECTOR2> tempUv;
	tempUv.push_back(D3DXVECTOR2(0, 0.66f));
	tempUv.push_back(D3DXVECTOR2(0.25f, 0.66f));
	tempUv.push_back(D3DXVECTOR2(0, 0.33f));
	tempUv.push_back(D3DXVECTOR2(0.25f, 0.33f));

	tempUv.push_back(D3DXVECTOR2(0.5f, 0.66f));
	tempUv.push_back(D3DXVECTOR2(0.5f, 0.33f));
	tempUv.push_back(D3DXVECTOR2(0.75f, 0.66f));
	tempUv.push_back(D3DXVECTOR2(0.75f, 0.33f));

	tempUv.push_back(D3DXVECTOR2(1, 0.66f));
	tempUv.push_back(D3DXVECTOR2(1, 0.33f));

	tempUv.push_back(D3DXVECTOR2(0.25f, 1));
	tempUv.push_back(D3DXVECTOR2(0.5f, 1));

	tempUv.push_back(D3DXVECTOR2(0.25f, 0));
	tempUv.push_back(D3DXVECTOR2(0.5f, 0));

	for (UINT i = 0; i < tempPos.size(); i++)
	{
		VertexTextureNormal temp;
		temp.Position = tempPos[i];
		temp.Normal = tempNorm[i];
		temp.Uv = tempUv[i];
		vCubeVertices.push_back(temp);
	}

	vCubeIndices.assign(3 * 12, 0);
	vCubeIndices = {
		0, 2, 1, // front
		1, 2, 3,
		4, 5, 6, // back
		5, 7, 6,
		6, 7, 8, //top
		7, 9 ,8,
		1, 3, 4, //bottom
		3, 5, 4,
		1, 11,10,// left
		1, 4, 11,
		3, 12, 5,//right
		5, 12, 13
	};
#endif
}

void DebugDraw::SettingObjSphere()
{
	//Draw Sphere
	UINT latitudeBands = 24;
	UINT longitudeBands = 24;
	float radius = 1.0f;

	for (UINT lat = 0; lat <= latitudeBands; lat++)
	{
		float theta = (float)lat * Math::PI / (float)latitudeBands;
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for (UINT lng = 0; lng <= longitudeBands; lng++)
		{
			VertexTextureNormal data;

			float phi = (float)lng * 2.0f * Math::PI / (float)longitudeBands;
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);

			D3DXVECTOR3 norm;
			norm.x = cosPhi * sinTheta;
			norm.y = cosTheta;
			norm.z = sinPhi * sinTheta;

			D3DXVECTOR2 uv;
			uv.x = 1.0f - ((float)lng / (float)longitudeBands);
			uv.y = (float)lat / (float)latitudeBands;

			D3DXVECTOR3 pos;
			pos = norm * radius;

			data.Position = pos;
			data.Normal = norm;
			data.Uv = uv;
			vSphereVertices.push_back(data);
		}
	}


	for (UINT lat = 0; lat < latitudeBands; lat++)
	{
		for (UINT lng = 0; lng < longitudeBands; lng++)
		{
			UINT first = (lat * (longitudeBands + 1)) + lng;
			UINT second = first + longitudeBands + 1;

			vSphereIndices.push_back(first);
			vSphereIndices.push_back(first + 1);
			vSphereIndices.push_back(second);
			vSphereIndices.push_back(second);
			vSphereIndices.push_back(first + 1);
			vSphereIndices.push_back(second + 1);
		}
	}
}

void DebugDraw::SettingObjCapsule()
{
	float radius = 1.0f;
	float height = 1.0f;
	D3DXVECTOR3 center = { 0, 0, 0 };

	int band = 30.0f;
	float theta = (Math::PI / 4.0f) / (float)band;

	//Up
	for (UINT i = 0; i < band; i++)
	{
		VertexTextureNormal v;
		float angle = (float)i * theta;
		D3DXVECTOR3 pos = D3DXVECTOR3(
			cosf(angle) * radius, sinf(angle) * radius, 0);

		v.Position = pos;

		vCapsuleVertices.push_back(v);
		vCapsuleIndices.push_back(i);
	}

	//Down

	//Center
}

