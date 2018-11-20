#include "stdafx.h"
#include "Model.h"
#include "ModelBone.h"
#include "ModelMesh.h"

Model::Model()
{
	buffer = new BoneBuffer();
}

Model::~Model()
{
	SAFE_DELETE(buffer);

	for (Material* material : materials)
		SAFE_DELETE(material);

	for (ModelBone* bone : bones)
		SAFE_DELETE(bone);

	for (ModelMesh* mesh : meshes)
		SAFE_DELETE(mesh);
}

inline Material * Model::MaterialByName(wstring name)
{
	for (Material* material : materials)
	{
		if (material->Name() == name)
			return material;
	}
}

ModelBone * Model::BoneByName(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->Name() == name)
			return bone;
	}

	return NULL;
}

ModelMesh * Model::MeshByName(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->Name() == name)
			return mesh;
	}

	return NULL;
}

ModelBone * Model::AddBone()
{
	ModelBone* _bone = new ModelBone();
	_bone->index = bones.size();
	_bone->name = L"AddBone::" + to_wstring(_bone->index);
	_bone->parentIndex = -1;
	D3DXMatrixIdentity(&_bone->local);
	D3DXMatrixIdentity(&_bone->global);

	bones.push_back(_bone);

	return _bone;
}

void Model::CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms)
{
	D3DXMATRIX w;
	D3DXMatrixIdentity(&w);

	CopyGlobalBoneTo((D3DXMATRIX *)&transforms[0], w);
}

void Model::CopyGlobalBoneTo(D3DXMATRIX * transforms, D3DXMATRIX & w)
{
	UINT boneSize = bones.size();

	for (size_t i = 0; i < boneSize; i++)
	{
		ModelBone* bone = bones[i];

		if (bone->Parent() != NULL)
		{
			int index = bone->parent->index;
			transforms[i] = bone->local * transforms[index];
		}
		else
		{
			transforms[i] = bone->local * w;
		}
	}
}

void Models::Create()
{
}

void Models::Delete()
{
	for (pair<wstring, vector<Material *>> temp : materialMap)
	{
		for (Material* material : temp.second)
			SAFE_DELETE(material);
	}

	for (pair<wstring, MeshData> temp : meshDataMap)
	{
		MeshData data = temp.second;

		for (ModelBone* bone : data.Bones)
			SAFE_DELETE(bone);

		for (ModelMesh* mesh : data.Meshes)
			SAFE_DELETE(mesh);
	}
}
