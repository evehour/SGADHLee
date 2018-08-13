#include "stdafx.h"
#include "ExeTankGame.h"
#include "../Objects/GameTank.h"
#include "../Objects/GameBullet.h"
#include "Physics/Collision.h"

ExeTankGame::ExeTankGame(ExecuteValues * values)
	: Execute(values)
{
	tank = new GameTank(Models + L"/Tank/", L"Tank.material");
}

ExeTankGame::~ExeTankGame()
{
	SAFE_DELETE(tank);

	for (GameBullet* bullet : bullets)
		SAFE_DELETE(bullet);

	for (GameModel* enemy : enemys)
		SAFE_DELETE(enemy);
}

void ExeTankGame::Update()
{
	D3DXMATRIX R, T;
	D3DXVECTOR3 _dir = D3DXVECTOR3(0, 0, 1) * 10.0f * Time::Get()->Delta();

	D3DXMatrixIdentity(&R);
	D3DXMatrixIdentity(&T);

	// 차체 제어
	if (!Mouse::Get()->Press(1))
	{
		if (Keyboard::Get()->Press('A'))
		{
			D3DXMatrixRotationY(&R, Math::ToRadian(-100.0f) * Time::Get()->Delta());
		}
		else if (Keyboard::Get()->Press('D'))
		{
			D3DXMatrixRotationY(&R, Math::ToRadian(100.0f) * Time::Get()->Delta());
		}

		if (Keyboard::Get()->Press('W'))
		{
			tank->SetTankState(TANK_STATE_MOVE);
			tank->SetWheelDir(true);
			_dir = -_dir;
			D3DXMatrixTranslation(&T, _dir.x, _dir.y, _dir.z);
		}
		else if (Keyboard::Get()->Press('S'))
		{
			tank->SetTankState(TANK_STATE_MOVE);
			tank->SetWheelDir(false);
			D3DXMatrixTranslation(&T, _dir.x, _dir.y, _dir.z);
		}

		if (Keyboard::Get()->Up('W') || Keyboard::Get()->Up('S'))
		{
			tank->SetTankState(TANK_STATE_IDLE);
		}
	}

	// 포신 제어
	if (Keyboard::Get()->Press('Q'))
	{
		tank->RotateTurret(-(Math::ToRadian(80) * Time::Delta()));
	}
	else if (Keyboard::Get()->Press('E'))
	{
		tank->RotateTurret((Math::ToRadian(80) * Time::Delta()));
	}
	tank->World((R * T) * tank->World());


	///////////////////////////////////////////////////////////////////////////
	// 포탄부분

	// 발사 처리
	if (Keyboard::Get()->Down(VK_SPACE))
	{
		GameBullet* b = new GameBullet(Materials + L"/Meshes/", L"Sphere.material");
		b->SetLaunchMatrix(tank->GetTurretMatrix());

		bullets.push_back(b);
	}


	tank->Update();

	vector<GameBullet *>::iterator iterBullets;
	iterBullets = bullets.begin();
	while (iterBullets != bullets.end())
	{
		vector<GameModel *>::iterator iterEnemys;
		bool canDel = false;
		for (iterEnemys = enemys.begin(); iterEnemys != enemys.end();)
		{
			bool b = Collision::IntersectRectToRect(
				*(GameModel*)(*iterBullets),
				*(*iterEnemys)
			);

			if (b)
			{
				canDel = true;
				iterEnemys = enemys.erase(iterEnemys);
				break;
			}
			++iterEnemys;
		}

		if (canDel)
			iterBullets = bullets.erase(iterBullets);
		else
		{
			(*iterBullets)->Update();
			++iterBullets;
		}
	}

	for (GameModel* enemy : enemys)
		enemy->Update();

}

void ExeTankGame::PreRender()
{
	
}

void ExeTankGame::Render()
{

	tank->Render();

	for (GameModel* bullet : bullets)
		bullet->Render();

	for (GameModel* enemy : enemys)
		enemy->Render();
}

void ExeTankGame::PostRender()
{
	float mapSize = 128.0f;

	ImGui::Begin("Model", 0, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Cube", "Create Cube Model")) {
				GameModel* p = new GameModel(
					Materials + L"Meshes/", L"Cube.material",
					Models + L"Meshes/", L"Cube.mesh"
				);
				p->Position(D3DXVECTOR3(
					Math::Random(0.0f, mapSize),
					3.0f,
					Math::Random(0.0f, mapSize)
				));

				enemys.push_back(p);
			}
			if (ImGui::MenuItem("Sphere", "Create Sphere Model")) {
				GameModel* p = new GameModel(
					Materials + L"Meshes/", L"Sphere.material",
					Models + L"Meshes/", L"Sphere.mesh"
				);
				p->Position(D3DXVECTOR3(
					Math::Random(0.0f, mapSize),
					3.0f,
					Math::Random(0.0f, mapSize)
				));

				enemys.push_back(p);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();
}

void ExeTankGame::ResizeScreen()
{
}

