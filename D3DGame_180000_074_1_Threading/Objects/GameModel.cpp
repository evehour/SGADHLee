#include "stdafx.h"
#include "GameModel.h"

#include "../Model/ModelBone.h"


GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, float frameLimit)
	: shader(NULL)
	, matFolder(matFolder), matFile(matFile), meshFolder(meshFolder), meshFile(meshFile)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	boneCount = model->BoneCount();

	if (frameLimit <= FLT_MIN)
		this->frameLimit = 0.0f;
	else
		this->frameLimit = 1.0f / frameLimit;

	this->frameStore = 0.0f;

	for (UINT i = 0; i < boneCount; i++)
	{
		defaultBoneLocalMatrix.push_back(model->Bones()[i]->Local());
		defaultBoneGlobalMatrix.push_back(model->Bones()[i]->Global());
	}

	boneTransforms = new D3DXMATRIX[boneCount];
}

GameModel::~GameModel()
{
	SAFE_DELETE_ARRAY(boneTransforms);
	
	SAFE_DELETE(shader);
	SAFE_DELETE(model);
}

thread* GameModel::UpdateThread()
{
	std::thread::id nobody;
	if (updateThreader.get_id() != nobody)
	{
		updateThreader = std::thread([=] { this->Update(); });
	}

	return &updateThreader;
	//return std::thread(&GameModel::Update, this);
}

void GameModel::Update()
{
	// Frame Limit
	//{
	//	if (frameLimit > FLT_MIN)
	//	{
	//		frameStore += Time::Delta();
	//		if (frameStore < frameLimit) return;
	//		frameStore -= frameLimit;
	//	}
	//}

	UpdateWorld();
}

void GameModel::Render()
{
	for (Material* material : model->Materials())
	{
		const float* data = boneTransforms[0];
		material->GetShader()->AsMatrix("Bones")->SetMatrixArray(data, 0, boneCount);
	}

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->Render();
	}
}

void GameModel::SetShader(Shader * shader)
{
	for (Material* material : model->Materials())
		material->SetShader(shader);
}

void GameModel::SetDiffuse(float r, float g, float b, float a)
{
	SetDiffuse(D3DXCOLOR(r, g, b, a));
}

void GameModel::SetDiffuse(D3DXCOLOR & color)
{
	for (Material* material : model->Materials())
		material->SetDiffuse(color);
}

void GameModel::SetDiffuseMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetDiffuseMap(file);
}

void GameModel::SetSpecularMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetSpecularMap(file);
}

void GameModel::SetNormalMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetNormalMap(file);
}

void GameModel::SetDetailMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetDetailMap(file);
}

void GameModel::SetSpecular(float r, float g, float b, float a)
{
	SetSpecular(D3DXCOLOR(r, g, b, a));
}

void GameModel::SetSpecular(D3DXCOLOR & color)
{
	for (Material* material : model->Materials())
		material->SetSpecular(color);
}

void GameModel::SetShininess(float val)
{
	for (Material* material : model->Materials())
		material->SetShininess(val);
}

void GameModel::UpdateWorld()
{
	__super::UpdateWorld();

	D3DXMATRIX t = Transformed();
	model->CopyGlobalBoneTo(boneTransforms, t);
}

bool GameModel::IsPick(D3DXVECTOR3 & origin, D3DXVECTOR3 & direction, OUT D3DXVECTOR3 & position)
{
#if false
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
		D3DXMATRIX boneWorld = bone->Global() * objectWorld;
		D3DXVECTOR3 dStart, dDir;

		D3DXVec3TransformCoord(&dStart, &start, &objectWorld);
		D3DXVec3TransformNormal(&dDir, &dir, &objectWorld);
		D3DXVec3Normalize(&dDir, &dDir);

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
					&dStart, &dDir,
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
#else
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
			vector<VertexTextureNormalTangentBlend> vertices = part->GetVertices();
			for (UINT i = 0; i < vertices.size(); i += 3)
			{
				D3DXVECTOR3 p[3];
				D3DXVec3TransformCoord(&p[0], &vertices[i + 0].Position, &boneWorld);
				D3DXVec3TransformCoord(&p[1], &vertices[i + 1].Position, &boneWorld);
				D3DXVec3TransformCoord(&p[2], &vertices[i + 2].Position, &boneWorld);
				if (D3DXIntersectTri(
					&p[0], &p[1], &p[2],
					&start, &dir,
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
#endif
}

void GameModel::GetAAABB(std::vector<D3DXVECTOR3>& aabbBox)
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

bool GameModel::IsPickPart(D3DXVECTOR3 & origin, D3DXVECTOR3 & direction, OUT int & partIdx, OUT D3DXVECTOR3 * position)
{
	D3DXVECTOR3 pickPosition(0, 0, 0);
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
			vector<VertexTextureNormalTangentBlend> vertices = part->GetVertices();
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
					D3DXVec3TransformCoord(&b.Position, &b.Position, &objectWorld);
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
	if(position != NULL) *position = retBuff.Position;
	partIdx = retBuff.PartIdx;
	return true;
}

void GameModel::SetBoneMatrixByIdx(const int & boneIdx, const D3DXMATRIX & matrix)
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

string GameModel::GetBoneNameByIdx(const int & boneIdx)
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

D3DXMATRIX GameModel::GetBoneLocalMatrixByIdx(const int & boneIdx)
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

void GameModel::SettingDefaultPose()
{
	for (UINT i = 0; i < model->Bones().size(); i++)
	{
		model->Bones()[i]->Local(defaultBoneLocalMatrix[i]);
		model->Bones()[i]->Global(defaultBoneGlobalMatrix[i]);
	}
}

void GameModel::SetBoundSpace()
{
	float min = -std::numeric_limits<float>().infinity(), max = std::numeric_limits<float>().infinity();
	D3DXVECTOR3 vecMin(max, max, max), vecMax(min, min, min);

	for (ModelMesh* modelMesh : model->Meshes())
	{
		ModelBone* bone = modelMesh->ParentBone();
		D3DXMATRIX w = bone->Global();
		for (ModelMeshPart* part : modelMesh->GetMeshParts())
		{
			vector<VertexTextureNormalTangentBlend> vertices = part->GetVertices();
			for (VertexTextureNormalTangentBlend data : vertices)
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

void GameModel::SetBoneList()
{
	for (ModelMesh* modelMesh : model->Meshes())
	{
		ModelBone* bone = modelMesh->ParentBone();

		boneList.push_back(make_pair(bone->Index(), make_pair(bone, bone->Local())));
	}
}

ModelBone * GameModel::AddBone()
{
	ModelBone* _b = model->AddBone();
	D3DXMATRIX _m;
	D3DXMatrixIdentity(&_m);

	defaultBoneGlobalMatrix.push_back(_m);
	defaultBoneLocalMatrix.push_back(_m);

	D3DXMATRIX* _pM;
	_pM = new D3DXMATRIX[boneCount];
	memcpy(_pM, boneTransforms, boneCount - 1);
	//memcpy_s(_pM, model->BoneCount() - 1, boneTransforms, boneCount - 1);
	D3DXMatrixIdentity(&_pM[boneCount - 1]);

	SAFE_DELETE_ARRAY(boneTransforms);
	boneTransforms = _pM;

	return _b;
}
