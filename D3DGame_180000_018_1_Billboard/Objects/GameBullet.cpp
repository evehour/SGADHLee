#include "stdafx.h"
#include "GameBullet.h"

GameBullet::GameBullet(wstring matFolder, wstring matFile)
	: GameModel(matFolder, matFile, Models + L"Meshes/", L"Sphere.mesh")
{
	shader = new Shader(Shaders + L"017_Sphere.hlsl");
	for (Material* material : model->Materials())
	{
		material->SetDiffuse(D3DXCOLOR(1, 0, 0, 1));
		material->SetShader(shader);
	}
}

GameBullet::~GameBullet()
{
}

void GameBullet::Update()
{
	__super::Update();

	D3DXVECTOR3 bulletForward(0, 0, 1), bulletDir, bulletPos;
	D3DXMATRIX T, W;

	D3DXVec3TransformNormal(&bulletDir, &bulletForward, &launchMatrix);
	D3DXVec3Normalize(&bulletDir, &bulletDir);
	bulletPos = (bulletDir * 50.0f * Time::Get()->Delta()) * -1.0f;
	D3DXMatrixTranslation(&T, bulletPos.x, bulletPos.y, bulletPos.z);
	W = T * World();
	Position(W._41, W._42, W._43);
}

void GameBullet::Render()
{
	__super::Render();
}

void GameBullet::SetLaunchMatrix(D3DXMATRIX & matrix)
{
	launchMatrix = matrix;
	Position(launchMatrix._41, launchMatrix._42, launchMatrix._43);
}
