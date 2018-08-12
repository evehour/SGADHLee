#include "stdafx.h"
#include "GameModel.h"
#include "Model/ModelMeshPart.h"
#ifdef __DEBUG_MODE__
#include "Physics/LineMake.h"
#endif

Shader* GameModel::shader = false;
bool GameModel::isCreateShader = false;

GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, Bound_Type boundType)
	: velocity(0, 0, 0)
	, center(0, 0, 0), boundSize(0, 0, 0), radius(0.0f)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	if (!isCreateShader)
	{
		isCreateShader = true;
		shader = new Shader(Shaders + L"010_1_Model.hlsl");
	}
	for (Material* material : model->Materials())
		material->SetShader(shader);

	D3DXMatrixIdentity(&matRotation);

	renderBuffer = new RenderBuffer();
	SetBoundSpace();
#ifdef __DEBUG_MODE__
	gizmo = new LineMake();

	gizmo->AddLine(boundSpace[0], boundSpace[1]);
	gizmo->AddLine(boundSpace[1], boundSpace[3]);
	gizmo->AddLine(boundSpace[3], boundSpace[2]);
	gizmo->AddLine(boundSpace[2], boundSpace[0]);

	gizmo->AddLine(boundSpace[4], boundSpace[5]);
	gizmo->AddLine(boundSpace[5], boundSpace[7]);
	gizmo->AddLine(boundSpace[7], boundSpace[6]);
	gizmo->AddLine(boundSpace[6], boundSpace[4]);

	gizmo->AddLine(boundSpace[0], boundSpace[4]);
	gizmo->AddLine(boundSpace[1], boundSpace[5]);
	gizmo->AddLine(boundSpace[2], boundSpace[6]);
	gizmo->AddLine(boundSpace[3], boundSpace[7]);
	gizmo->UpdateBuffer();

	gizmoAABB = new LineMake();
#endif
}

GameModel::~GameModel()
{
#ifdef __DEBUG_MODE__
	SAFE_DELETE(gizmoAABB);
	SAFE_DELETE(gizmo);
#endif
	SAFE_DELETE(renderBuffer);

	if (isCreateShader)
	{
		isCreateShader = false;
		SAFE_DELETE(shader);
	}
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
#ifdef __DEBUG_MODE__
	gizmo->SetWorld(World());
	{
		std::vector<D3DXVECTOR3> _vertices;
		GetAAABB(_vertices);
		gizmoAABB->ClearBuffer();

		gizmoAABB->AddLine(_vertices[0], _vertices[1]);
		gizmoAABB->AddLine(_vertices[1], _vertices[3]);
		gizmoAABB->AddLine(_vertices[3], _vertices[2]);
		gizmoAABB->AddLine(_vertices[2], _vertices[0]);

		gizmoAABB->AddLine(_vertices[4], _vertices[5]);
		gizmoAABB->AddLine(_vertices[5], _vertices[7]);
		gizmoAABB->AddLine(_vertices[7], _vertices[6]);
		gizmoAABB->AddLine(_vertices[6], _vertices[4]);

		gizmoAABB->AddLine(_vertices[0], _vertices[4]);
		gizmoAABB->AddLine(_vertices[1], _vertices[5]);
		gizmoAABB->AddLine(_vertices[2], _vertices[6]);
		gizmoAABB->AddLine(_vertices[3], _vertices[7]);
		gizmoAABB->UpdateBuffer();
	}
#endif
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

#ifdef __DEBUG_MODE__
	gizmo->Render();
	gizmoAABB->Render();
#endif
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
	//aabbBox = boundSpace;
	//D3DXVec3TransformCoordArray(&aabbBox[0], sizeof(D3DXVECTOR3), &aabbBox[0], sizeof(D3DXVECTOR3), &World(), aabbBox.size());
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
