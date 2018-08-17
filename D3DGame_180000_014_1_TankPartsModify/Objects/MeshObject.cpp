#include "stdafx.h"
#include "MeshObject.h"
#include "Model/ModelMeshPart.h"

MeshObject::MeshObject(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, D3DXCOLOR diffuseColor)
	: GameModel(matFolder, matFile, meshFolder, meshFile)
{
	shader = new Shader(Shaders + L"014_Model.hlsl");
	for (Material* material : model->Materials())
	{
		material->SetShader(shader);
	}

	SetBoundSpace();
	SetBoneList();
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

	for (ModelMesh* modelMesh : model->Meshes())
	{
		ModelBone* bone = modelMesh->ParentBone();
		D3DXMATRIX boneWorld = bone->Global();
		for (ModelMeshPart* part : modelMesh->GetMeshParts())
		{
			vector<ModelVertexType> vertices = part->GetVertices();
			for (UINT i = 0; i < vertices.size(); i += 3)
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

bool MeshObject::IsPickPart(D3DXVECTOR3 & origin, D3DXVECTOR3 & direction, OUT int & partIdx, OUT D3DXVECTOR3 & position)
{
	float u, v, distance;
	D3DXVECTOR3 start, dir;
	D3DXMATRIX objectWorld = World();
	D3DXMATRIX invWorld;
	D3DXMatrixInverse(&invWorld, NULL, &objectWorld);

	D3DXVec3TransformCoord(&start, &origin, &invWorld);
	D3DXVec3TransformNormal(&dir, &direction, &invWorld);
	D3DXVec3Normalize(&dir, &dir);

	struct buff
	{
		int PartIdx;
		D3DXVECTOR3 Position;
		float Distance;

		buff()
		{
			Distance = -1.0f;
		}
	};
	vector<buff> _vBuff;

	for (ModelMesh* modelMesh : model->Meshes())
	{
		ModelBone* bone = modelMesh->ParentBone();
		D3DXMATRIX boneWorld = boneTransforms[bone->Index()];
		bool find = false;
		for (ModelMeshPart* part : modelMesh->GetMeshParts())
		{
			vector<ModelVertexType> vertices = part->GetVertices();
			for (UINT i = 0; i < vertices.size(); i += 3)
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
					buff b;
					b.Position = p[0] + (u * (p[1] - p[0])) + (v * (p[2] - p[0]));
					D3DXVec3TransformCoord(&position, &position, &objectWorld);
					b.PartIdx = bone->Index();
					b.Distance = distance;
					_vBuff.push_back(b);
					find = true;
					break;
				}
			}

			if (find) break;
		}
	}

	if (_vBuff.size() < 1) return false;
	buff retBuff = _vBuff[0];
	for (UINT i = 1; i < _vBuff.size(); i++)
	{
		if (_vBuff[i].Distance < retBuff.Distance)
			retBuff = _vBuff[i];
	}

	position = retBuff.Position;
	partIdx = retBuff.PartIdx;
	return true;
}

void MeshObject::SetBoneMatrixByIdx(const int & boneIdx, const D3DXMATRIX & matrix)
{	
	for (std::vector<pair<int, pair<class ModelBone *, D3DXMATRIX>>>::iterator iter = boneList.begin();
		iter != boneList.end();
		iter++)
	{
		if (iter->first == boneIdx)
		{
			D3DXMATRIX dMatrix = iter->second.second;
			iter->second.first->Local((D3DXMATRIX)matrix);
			break;
		}
	}
}

string MeshObject::GetBoneNameByIdx(const int & boneIdx)
{
	for (std::vector<pair<int, pair<class ModelBone *, D3DXMATRIX>>>::iterator iter = boneList.begin();
		iter != boneList.end();
		iter++)
	{
		if (iter->first == boneIdx)
		{
			return String::ToString(iter->second.first->Name());
			break;
		}
	}

	return "";
}

D3DXMATRIX MeshObject::GetBoneLocalMatrixByIdx(const int & boneIdx)
{
	D3DXMATRIX matRet;
	D3DXMatrixIdentity(&matRet);

	for (std::vector<pair<int, pair<class ModelBone *, D3DXMATRIX>>>::iterator iter = boneList.begin();
		iter != boneList.end();
		iter++)
	{
		if (iter->first == boneIdx)
		{
			matRet = iter->second.first->Local();
			break;
		}
	}
	return matRet;
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

void MeshObject::SetBoneList()
{
	for (ModelMesh* modelMesh : model->Meshes())
	{
		ModelBone* bone = modelMesh->ParentBone();
		
		boneList.push_back(make_pair(bone->Index(), make_pair(bone, bone->Local())));
	}
}
