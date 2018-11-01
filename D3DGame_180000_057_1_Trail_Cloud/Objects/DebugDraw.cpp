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
	vector<D3DXVECTOR3> _vTmpVertices;
	this->objNum = objNum;

	switch (objNum)
	{
	default:
	case DRAW_OBJECT_TYPE_SPHERE:
		vTargetVertices = &vSphereVertices;
		vTargetIndices = &vSphereIndices;

		for (UINT i = 0; i < vTargetVertices->size(); i++)
			_vTmpVertices.push_back((*vTargetVertices)[i].Position);

		line->MeshData(_vTmpVertices, *vTargetIndices, objNum);
		line->SetColor(D3DXCOLOR(1, 1, 0, 1));
		break;
	case DRAW_OBJECT_TYPE_CUBE:
		vTargetVertices = &vCubeVertices;
		vTargetIndices = &vCubeIndices;

		for (UINT i = 0; i < vTargetVertices->size(); i++)
			_vTmpVertices.push_back((*vTargetVertices)[i].Position);

		line->MeshData(_vTmpVertices, *vTargetIndices, objNum);
		line->SetColor(D3DXCOLOR(1, 1, 0, 1));
		break;
	case DRAW_OBJECT_TYPE_CAPSULE:
		vTargetVertices = &vCapsuleVertices;
		vTargetIndices = &vCapsuleIndices;
		line->SetColor(D3DXCOLOR(1, 1, 0, 1));
		break;
	}
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

void DebugDraw::SettingObjCapsule2()
{
	float radius = 0.5f;
	float height = 1.0f;
	int nextIndice;
	D3DXVECTOR3 center = { 0, 0, 0 };

	int band = 60;
	float theta = Math::PI / (float)band;

	nextIndice = 0;
	// 첫번째
	{
		//Up
		for (int i = 0; i <= band; i++)
		{
			VertexTextureNormal v;
			float angle = (float)i * theta;

			v.Position = D3DXVECTOR3(
				cosf(angle) * radius, (height / 2.0f) + sinf(angle) * radius, 0);

			vCapsuleVertices.push_back(v);
		}

		//Down
		for (int i = 0; i <= band; i++)
		{
			VertexTextureNormal v;
			float angle = (float)i * theta + Math::PI;

			v.Position = D3DXVECTOR3(
				cosf(angle) * radius, -(height / 2.0f) + sinf(angle) * radius, 0);

			vCapsuleVertices.push_back(v);
		}

		for (UINT i = 0; i < vCapsuleVertices.size() - 1; i++)
		{
			vCapsuleIndices.push_back(i);
			vCapsuleIndices.push_back(i + 1);
			line->AddLine(vCapsuleVertices[vCapsuleIndices[i * 2]].Position, vCapsuleVertices[vCapsuleIndices[i * 2 + 1]].Position);
		}

		vCapsuleIndices.push_back(vCapsuleIndices[vCapsuleIndices.size() - 1]);
		vCapsuleIndices.push_back(0);
		line->AddLine(vCapsuleVertices[vCapsuleIndices[vCapsuleIndices.size() - 1]].Position, vCapsuleVertices[vCapsuleIndices[vCapsuleIndices.size() - 2]].Position);
	}


	{
		theta = Math::PI / 4.0f;
		D3DXMATRIX _matWorld;
		int verticesSize = vCapsuleVertices.size();
		int indicesSize = vCapsuleIndices.size();
		for (UINT i = 1; i < 4; i++)
		{
			D3DXMatrixRotationY(&_matWorld, (theta * (float)i));
			for (int j = 0; j < verticesSize; j++)
			{
				VertexTextureNormal v;
				D3DXVec3TransformCoord(&v.Position, &vCapsuleVertices[j].Position, &_matWorld);
				vCapsuleVertices.push_back(v);
			}

			for (int j = 0; j < verticesSize - 1; j++)
			{
				vCapsuleIndices.push_back((verticesSize * i) + j);
				vCapsuleIndices.push_back((verticesSize * i) + j + 1);
				line->AddLine(vCapsuleVertices[vCapsuleIndices[(indicesSize * i) + (j * 2)]].Position, vCapsuleVertices[vCapsuleIndices[(indicesSize * i) + (j * 2) + 1]].Position);
			}

			vCapsuleIndices.push_back(vCapsuleIndices[vCapsuleIndices.size() - 1]);
			vCapsuleIndices.push_back(vCapsuleIndices[(indicesSize * i)]);
			line->AddLine(vCapsuleVertices[vCapsuleIndices[vCapsuleIndices.size() - 1]].Position, vCapsuleVertices[vCapsuleIndices[vCapsuleIndices.size() - 2]].Position);
		}
	}

	{
		theta = (Math::PI * 2.0f) / (float)band;

		int verticesSize = vCapsuleVertices.size();
		int indicesSize = vCapsuleIndices.size();

		// top circle
		for (int i = 0; i <= band; i++)
		{
			VertexTextureNormal v;
			float angle = (float)i * theta;
			v.Position = D3DXVECTOR3(cosf(angle) * radius, (height / 2.0f), sinf(angle) * radius);
			vCapsuleVertices.push_back(v);
		}

		for (int j = 0; j < band; j++)
		{
			vCapsuleIndices.push_back((verticesSize) + j);
			vCapsuleIndices.push_back((verticesSize) + j + 1);
			line->AddLine(vCapsuleVertices[vCapsuleIndices[(indicesSize) + (j * 2)]].Position, vCapsuleVertices[vCapsuleIndices[(indicesSize) + (j * 2) + 1]].Position);
		}

		//center circle
		verticesSize = vCapsuleVertices.size();
		indicesSize = vCapsuleIndices.size();
		for (int i = 0; i <= band; i++)
		{
			VertexTextureNormal v;
			float angle = (float)i * theta;
			v.Position = D3DXVECTOR3(cosf(angle) * radius, 0, sinf(angle) * radius);
			vCapsuleVertices.push_back(v);
		}

		for (int j = 0; j < band; j++)
		{
			vCapsuleIndices.push_back((verticesSize)+j);
			vCapsuleIndices.push_back((verticesSize)+j + 1);
			line->AddLine(vCapsuleVertices[vCapsuleIndices[(indicesSize)+(j * 2)]].Position, vCapsuleVertices[vCapsuleIndices[(indicesSize)+(j * 2) + 1]].Position);
		}

		//center circle
		verticesSize = vCapsuleVertices.size();
		indicesSize = vCapsuleIndices.size();
		for (int i = 0; i <= band; i++)
		{
			VertexTextureNormal v;
			float angle = (float)i * theta;
			v.Position = D3DXVECTOR3(cosf(angle) * radius, -(height / 2.0f), sinf(angle) * radius);
			vCapsuleVertices.push_back(v);
		}

		for (int j = 0; j < band; j++)
		{
			vCapsuleIndices.push_back((verticesSize)+j);
			vCapsuleIndices.push_back((verticesSize)+j + 1);
			line->AddLine(vCapsuleVertices[vCapsuleIndices[(indicesSize)+(j * 2)]].Position, vCapsuleVertices[vCapsuleIndices[(indicesSize)+(j * 2) + 1]].Position);
		}
	}

	//line->InputBuffer(vCapsuleVertices, vCapsuleIndices);
}

void DebugDraw::SettingObjCapsule()
{
	float radius = 0.5f;
	float height = 1.0f;
	int nextIndice;
	D3DXVECTOR3 center = { 0, 0, 0 };

	int band = 60;
	float theta = Math::PI / (float)band;

	nextIndice = 0;
	// 첫번째
	{
		//Up
		for (int i = 0; i <= band; i++)
		{
			VertexTextureNormal v;
			float angle = (float)i * theta;

			v.Position = D3DXVECTOR3(
				cosf(angle) * radius, (height / 2.0f) + sinf(angle) * radius, 0);

			vCapsuleVertices.push_back(v);
		}

		//Down
		for (int i = 0; i <= band; i++)
		{
			VertexTextureNormal v;
			float angle = (float)i * theta + Math::PI;

			v.Position = D3DXVECTOR3(
				cosf(angle) * radius, -(height / 2.0f) + sinf(angle) * radius, 0);

			vCapsuleVertices.push_back(v);
		}

		for (UINT i = 0; i < vCapsuleVertices.size() - 1; i++)
		{
			vCapsuleIndices.push_back(i);
			vCapsuleIndices.push_back(i + 1);
		}

		vCapsuleIndices.push_back(vCapsuleIndices[vCapsuleIndices.size() - 1]);
		vCapsuleIndices.push_back(0);
	}


	{
		theta = Math::PI / 4.0f;
		D3DXMATRIX _matWorld;
		int verticesSize = vCapsuleVertices.size();
		int indicesSize = vCapsuleIndices.size();
		for (UINT i = 1; i < 4; i++)
		{
			D3DXMatrixRotationY(&_matWorld, (theta * (float)i));
			for (int j = 0; j < verticesSize; j++)
			{
				VertexTextureNormal v;
				D3DXVec3TransformCoord(&v.Position, &vCapsuleVertices[j].Position, &_matWorld);
				vCapsuleVertices.push_back(v);
			}

			for (int j = 0; j < verticesSize - 1; j++)
			{
				vCapsuleIndices.push_back((verticesSize * i) + j);
				vCapsuleIndices.push_back((verticesSize * i) + j + 1);
			}

			vCapsuleIndices.push_back(vCapsuleIndices[vCapsuleIndices.size() - 1]);
			vCapsuleIndices.push_back(vCapsuleIndices[(indicesSize * i)]);
		}
	}

	{
		theta = (Math::PI * 2.0f) / (float)band;

		int verticesSize = vCapsuleVertices.size();
		int indicesSize = vCapsuleIndices.size();

		// top circle
		for (int i = 0; i <= band; i++)
		{
			VertexTextureNormal v;
			float angle = (float)i * theta;
			v.Position = D3DXVECTOR3(cosf(angle) * radius, (height / 2.0f), sinf(angle) * radius);
			vCapsuleVertices.push_back(v);
		}

		for (int j = 0; j < band; j++)
		{
			vCapsuleIndices.push_back((verticesSize)+j);
			vCapsuleIndices.push_back((verticesSize)+j + 1);
		}

		//center circle
		verticesSize = vCapsuleVertices.size();
		indicesSize = vCapsuleIndices.size();
		for (int i = 0; i <= band; i++)
		{
			VertexTextureNormal v;
			float angle = (float)i * theta;
			v.Position = D3DXVECTOR3(cosf(angle) * radius, 0, sinf(angle) * radius);
			vCapsuleVertices.push_back(v);
		}

		for (int j = 0; j < band; j++)
		{
			vCapsuleIndices.push_back((verticesSize)+j);
			vCapsuleIndices.push_back((verticesSize)+j + 1);
		}

		//center circle
		verticesSize = vCapsuleVertices.size();
		indicesSize = vCapsuleIndices.size();
		for (int i = 0; i <= band; i++)
		{
			VertexTextureNormal v;
			float angle = (float)i * theta;
			v.Position = D3DXVECTOR3(cosf(angle) * radius, -(height / 2.0f), sinf(angle) * radius);
			vCapsuleVertices.push_back(v);
		}

		for (int j = 0; j < band; j++)
		{
			vCapsuleIndices.push_back((verticesSize)+j);
			vCapsuleIndices.push_back((verticesSize)+j + 1);
		}
	}

	line->MeshData(vCapsuleVertices, vCapsuleIndices, 2);
	//line->InputBuffer(vCapsuleVertices, vCapsuleIndices);
}

