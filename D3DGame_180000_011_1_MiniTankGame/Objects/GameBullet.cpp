#include "stdafx.h"
#include "GameBullet.h"

GameBullet::GameBullet(wstring matFolder, wstring matFile)
	: GameModel(matFolder, matFile, Models + L"Meshes/", L"Sphere.mesh")
{
}

GameBullet::~GameBullet()
{
}

void GameBullet::Update()
{
	__super::Update();

	D3DXVECTOR3 bulletForward(0, 0, 1), bulletDir, bullletPos;
	D3DXMATRIX T;

	D3DXVec3TransformNormal(&bulletDir, &bulletForward, &launchMatrix);
	D3DXVec3Normalize(&bulletDir, &bulletDir);
	bullletPos = (bulletDir * 50.0f * Time::Get()->Delta()) * -1.0f;
	D3DXMatrixTranslation(&T, bullletPos.x, bullletPos.y, bullletPos.z);
	World(T * World());
}

void GameBullet::Render()
{
	__super::Render();
}

void GameBullet::SetLaunchMatrix(D3DXMATRIX & matrix)
{
	launchMatrix = matrix;
	D3DXMATRIX w;
	D3DXMatrixTranslation(&w, launchMatrix._41, launchMatrix._42, launchMatrix._43);
	World(w);
}
