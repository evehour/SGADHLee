#include "stdafx.h"
#include "GameModel.h"
#include "Model/ModelMeshPart.h"
#include "../Physics/LineMake.h"

GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: velocity(0, 0, 0)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	D3DXMatrixIdentity(&matRotation);

	renderBuffer = new RenderBuffer();

	//HW
	{
		SetBoundSpace();

		box = new LineMake();

		box->AddLine(boundSpace[0], boundSpace[1]);
		box->AddLine(boundSpace[1], boundSpace[3]);
		box->AddLine(boundSpace[3], boundSpace[2]);
		box->AddLine(boundSpace[2], boundSpace[0]);

		box->AddLine(boundSpace[4], boundSpace[5]);
		box->AddLine(boundSpace[5], boundSpace[7]);
		box->AddLine(boundSpace[7], boundSpace[6]);
		box->AddLine(boundSpace[6], boundSpace[4]);

		box->AddLine(boundSpace[0], boundSpace[4]);
		box->AddLine(boundSpace[1], boundSpace[5]);
		box->AddLine(boundSpace[2], boundSpace[6]);
		box->AddLine(boundSpace[3], boundSpace[7]);

		D3DXVECTOR3 _right, _up, _forw;
		_right = _up = _forw = center;
		_right.x	-= 6;
		_up.y		+= 6;
		_forw.z		-= 6;

		box->AddLine(center, _right);
		box->AddLine(center, _up);
		box->AddLine(center, _forw);
		box->UpdateBuffer();

		boxAABB = new LineMake();
	}
}

GameModel::~GameModel()
{
	SAFE_DELETE(boxAABB);
	SAFE_DELETE(box);

	SAFE_DELETE(renderBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(model);
}

void GameModel::Velocity(D3DXVECTOR3 & vec)
{
	velocity = vec;
}

D3DXVECTOR3 GameModel::Velocity()
{
	return velocity;
}

void GameModel::Update()
{
	CalcPosition();

	D3DXMATRIX t = Transformed();
	model->CopyGlobalBoneTo(boneTransforms, t);

	//HW
	{
		box->SetWorld(World());
		std::vector<D3DXVECTOR3> _vertices;
		GetAAABB(_vertices);
		boxAABB->ClearBuffer();

		boxAABB->AddLine(_vertices[0], _vertices[1]);
		boxAABB->AddLine(_vertices[1], _vertices[3]);
		boxAABB->AddLine(_vertices[3], _vertices[2]);
		boxAABB->AddLine(_vertices[2], _vertices[0]);

		boxAABB->AddLine(_vertices[4], _vertices[5]);
		boxAABB->AddLine(_vertices[5], _vertices[7]);
		boxAABB->AddLine(_vertices[7], _vertices[6]);
		boxAABB->AddLine(_vertices[6], _vertices[4]);

		boxAABB->AddLine(_vertices[0], _vertices[4]);
		boxAABB->AddLine(_vertices[1], _vertices[5]);
		boxAABB->AddLine(_vertices[2], _vertices[6]);
		boxAABB->AddLine(_vertices[3], _vertices[7]);
		boxAABB->UpdateBuffer();
	}
}

void GameModel::Render()
{
	model->Buffer()->Bones(&boneTransforms[0], boneTransforms.size());
	model->Buffer()->SetVSBuffer(2);

	for (ModelMesh* mesh : model->Meshes())
	{
		int index = mesh->ParentBoneIndex();

		renderBuffer->Data.Index = index;
		renderBuffer->SetVSBuffer(3);

		mesh->Render();
	}

	box->Render();
	boxAABB->Render();
}

void GameModel::Rotate(D3DXVECTOR2 amount)
{
	amount *= Time::Delta();


	D3DXMATRIX axis;
	D3DXMatrixRotationAxis(&axis, &Right(), amount.y);

	D3DXMATRIX Y;
	D3DXMatrixRotationY(&Y, Math::ToRadian(amount.x));

	matRotation = axis * Y;

	D3DXMATRIX R = World();
	World(matRotation * R);
}

void GameModel::CalcPosition()
{
	if (D3DXVec3Length(&velocity) <= 0.0f)
		return;


	D3DXVECTOR3 vec(0, 0, 0);
	if (velocity.z != 0.0f)
		vec += Direction() * velocity.z;

	if (velocity.y != 0.0f)
		vec += Up() * velocity.y;

	if (velocity.x != 0.0f)
		vec += Right() * velocity.x;

	D3DXVECTOR3 pos = Position() + vec * Time::Delta();
	Position(pos);
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
}
