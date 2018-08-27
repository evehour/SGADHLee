#include "stdafx.h"
#include "GameModel.h"
#include "../Model/ModelMeshPart.h"

GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: velocity(0, 0, 0)
	, shader(NULL)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	renderBuffer = new RenderBuffer();
}

GameModel::~GameModel()
{
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
			vector<ModelVertexType> vertices = part->GetVertices();
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
