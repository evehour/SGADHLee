#include "stdafx.h"
#include "MeshObject.h"
#include "Model/ModelMeshPart.h"

MeshObject::MeshObject(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, D3DXCOLOR diffuseColor)
	: GameModel(matFolder, matFile, meshFolder, meshFile)
{
	shader = new Shader(Shaders + L"017_Sphere.hlsl");
	for (Material* material : model->Materials())
	{
		material->SetDiffuse(diffuseColor);
		material->SetShader(shader);
	}

	SetBoundSpace();
}

MeshObject::~MeshObject()
{
	SAFE_DELETE(shader);
}

void MeshObject::Update()
{
	__super::Update();
}

void MeshObject::Render()
{
	__super::Render();
}

void MeshObject::GetAAABB(std::vector<D3DXVECTOR3>& aabbBox)
{
#if false
	float min = -std::numeric_limits<float>().infinity(), max = std::numeric_limits<float>().infinity();
	D3DXVECTOR3 dMin(max, max, max), dMax(min, min, min);

	aabbBox.clear();

	for (D3DXVECTOR3 vec : boundSpace)
	{
		D3DXVec3TransformCoord(&vec, &vec, &World());
		aabbBox.push_back(vec);
	}

	for (D3DXVECTOR3 vec : aabbBox)
	{
		if (dMin.x > vec.x) dMin.x = vec.x;
		if (dMin.y > vec.y) dMin.y = vec.y;
		if (dMin.z > vec.z) dMin.z = vec.z;

		if (dMax.x < vec.x) dMax.x = vec.x;
		if (dMax.y < vec.y) dMax.y = vec.y;
		if (dMax.z < vec.z) dMax.z = vec.z;
	}

	aabbBox.clear();
	aabbBox.reserve(8);
#else
	bool isFirst = true;
	D3DXVECTOR3 dMin, dMax;
	aabbBox.assign(boundSpace.begin(), boundSpace.end());

	for (D3DXVECTOR3 vec : aabbBox)
	{
		D3DXVec3TransformCoord(&vec, &vec, &World());
		if (isFirst)
		{
			dMin = vec;
			dMax = vec;
			isFirst = false;
			continue;
		}
		if (dMin.x > vec.x) dMin.x = vec.x;
		if (dMin.y > vec.y) dMin.y = vec.y;
		if (dMin.z > vec.z) dMin.z = vec.z;

		if (dMax.x < vec.x) dMax.x = vec.x;
		if (dMax.y < vec.y) dMax.y = vec.y;
		if (dMax.z < vec.z) dMax.z = vec.z;
	}

	aabbBox.clear();
#endif
	aabbBox.push_back(D3DXVECTOR3(dMin.x, dMin.y, dMin.z));
	aabbBox.push_back(D3DXVECTOR3(dMin.x, dMax.y, dMin.z));
	aabbBox.push_back(D3DXVECTOR3(dMax.x, dMin.y, dMin.z));
	aabbBox.push_back(D3DXVECTOR3(dMax.x, dMax.y, dMin.z));
	aabbBox.push_back(D3DXVECTOR3(dMin.x, dMin.y, dMax.z));
	aabbBox.push_back(D3DXVECTOR3(dMin.x, dMax.y, dMax.z));
	aabbBox.push_back(D3DXVECTOR3(dMax.x, dMin.y, dMax.z));
	aabbBox.push_back(D3DXVECTOR3(dMax.x, dMax.y, dMax.z));

	return;
}

bool MeshObject::IsPick(D3DXVECTOR3 & origin, D3DXVECTOR3 & direction, OUT D3DXVECTOR3 & position)
{
	float u, v, distance;
	D3DXVECTOR3 start, dir;
	D3DXMATRIX objectWorld = World();
	D3DXMATRIX invWorld;
	D3DXMatrixInverse(&invWorld, NULL, &objectWorld);

	D3DXVec3TransformCoord(&start, &origin, &invWorld);
	D3DXVec3TransformNormal(&dir, &direction, &invWorld);
	D3DXVec3Normalize(&dir, &dir);

	bool isFind = false;

	for (ModelMesh* modelMesh : model->Meshes())
	{
		ModelBone* bone = modelMesh->ParentBone();
		D3DXMATRIX boneWorld = bone->Global();
		for (ModelMeshPart* part : modelMesh->GetMeshParts())
		{
			vector<ModelVertexType> vertices = part->GetVertices();
			for (int i = 0; i < vertices.size(); i += 3)
			{
				D3DXVECTOR3 p[3];
				D3DXVec3TransformCoord(&p[0], &vertices[i + 0].Position, &boneWorld);
				D3DXVec3TransformCoord(&p[1], &vertices[i + 1].Position, &boneWorld);
				D3DXVec3TransformCoord(&p[2], &vertices[i + 2].Position, &boneWorld);
				if (D3DXIntersectTri(
					&p[0], &p[1], &p[2],
					&start, &direction,
					&u, &v, &distance))
				{
					position = p[0] + (u * (p[1] - p[0])) + (v * (p[2] - p[0]));
					D3DXVec3TransformCoord(&position, &position, &objectWorld);
					return true;
				}
			}
		}
	}
	return false;
}

void MeshObject::SetBoundSpace()
{
	float min = -std::numeric_limits<float>().infinity(), max = std::numeric_limits<float>().infinity();
	D3DXVECTOR3 vecMin(max, max, max), vecMax(min, min, min);

	for (ModelMesh* modelMesh : model->Meshes())
	{
		ModelBone* bone = modelMesh->ParentBone();
		D3DXMATRIX w = bone->Global();
		for (ModelMeshPart* part : modelMesh->GetMeshParts())
		{
			vector<ModelVertexType> vertices = part->GetVertices();
			for (ModelVertexType data : vertices)
			{
				D3DXVECTOR3 pos = data.Position;
				D3DXVec3TransformCoord(&pos, &pos, &w);
				if (vecMin.x > pos.x) vecMin.x = pos.x;
				if (vecMax.x < pos.x) vecMax.x = pos.x;

				if (vecMin.y > pos.y) vecMin.y = pos.y;
				if (vecMax.y < pos.y) vecMax.y = pos.y;

				if (vecMin.z > pos.z) vecMin.z = pos.z;
				if (vecMax.z < pos.z) vecMax.z = pos.z;
			}
		}
	}

	this->vecMin = vecMin;
	this->vecMax = vecMax;
	center = (vecMax + vecMin) / 2.0f;

	D3DXVECTOR3 temp = vecMax + center;
	if (temp.x < 0) temp.x *= -1;
	if (temp.y < 0) temp.y *= -1;
	if (temp.z < 0) temp.z *= -1;
	radius = max(temp.x, temp.y);
	radius = max(radius, temp.z);
	boundSize = temp * 2.0f;

	boundSpace.push_back(D3DXVECTOR3(vecMin.x, vecMin.y, vecMin.z));
	boundSpace.push_back(D3DXVECTOR3(vecMin.x, vecMax.y, vecMin.z));
	boundSpace.push_back(D3DXVECTOR3(vecMax.x, vecMin.y, vecMin.z));
	boundSpace.push_back(D3DXVECTOR3(vecMax.x, vecMax.y, vecMin.z));
	boundSpace.push_back(D3DXVECTOR3(vecMin.x, vecMin.y, vecMax.z));
	boundSpace.push_back(D3DXVECTOR3(vecMin.x, vecMax.y, vecMax.z));
	boundSpace.push_back(D3DXVECTOR3(vecMax.x, vecMin.y, vecMax.z));
	boundSpace.push_back(D3DXVECTOR3(vecMax.x, vecMax.y, vecMax.z));

	UINT indices[36]
	{
		0,1,2, 2,1,3,
		2,3,6, 6,3,7,
		6,7,5, 5,7,4,
		4,5,0, 0,5,1,
		1,5,3, 3,5,7,
		4,0,6, 6,0,2
	};

	for (int i = 0; i < 36; i++)
	{
		boundSpaceTries.push_back(boundSpace[indices[i]]);
	}
	
}
