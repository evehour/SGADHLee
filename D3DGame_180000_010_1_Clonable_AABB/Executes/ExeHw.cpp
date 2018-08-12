#include "stdafx.h"
#include "ExeHw.h"
#include "Objects/GameModel.h"
#include "Physics/Collision.h"

ExeHw::ExeHw(ExecuteValues * values)
	: Execute(values)
{
	tank = new GameModel
	(
		Models + L"/Tank/", L"Tank.material",
		Models + L"/Tank/", L"Tank.mesh",
		GameModel::BOUND_TYPE_BOX
	);

	vGameModel.reserve(65535);

	for (int i = 0; i < 50; i++)
	{
		if (i % 2 == 0)
		{
			vGameModel.push_back(
				new GameModel
				(
					Models + L"/Sphere/", L"Sphere.material",
					Models + L"/Sphere/", L"Sphere.mesh",
					GameModel::BOUND_TYPE_BOX
				)
			);
		}
		else
		{
			vGameModel.push_back(
				new GameModel
				(
					Models + L"/Cube/", L"Cube.material",
					Models + L"/Cube/", L"Cube.mesh",
					GameModel::BOUND_TYPE_BOX
				)
			);
		}
		vGameModel[i]->Position(D3DXVECTOR3(Math::Random(0, 128), 0, Math::Random(0, 128)));
	}
}

ExeHw::~ExeHw()
{
	std::vector<GameModel *>::iterator iter = vGameModel.begin();
	while (iter != vGameModel.end())
	{
		SAFE_DELETE(*iter);
		iter = vGameModel.erase(iter);
	}

	SAFE_DELETE(tank);
}

void ExeHw::Update()
{
	if (!Mouse::Get()->Down(0))
	{
		D3DXVECTOR3 _tankPos = tank->Position();
		D3DXVECTOR3 _dir = tank->Direction();
		D3DXVECTOR3 _up = tank->Up(); // Y축으로 돌려야 좌우 방향전환이라서
		D3DXMATRIX _matRotation;

		// 탱크 기본 방향이 아래를 향하고 있으므로 반대로 해야함
		if (Keyboard::Get()->Press('T'))
			tank->Position(_tankPos - (_dir * 30.0f * Time::Get()->Delta()));
		else if (Keyboard::Get()->Press('G'))
			tank->Position(_tankPos + (_dir * 30.0f * Time::Get()->Delta()));

		if (Keyboard::Get()->Press('F'))
		{
			D3DXMatrixRotationAxis(&_matRotation, &-_up, Math::ToRadian(100.0f) * Time::Get()->Delta());
			tank->World(_matRotation * tank->World());
		}
		else if (Keyboard::Get()->Press('H'))
		{
			D3DXMatrixRotationAxis(&_matRotation, &_up, Math::ToRadian(100.0f) * Time::Get()->Delta());
			tank->World(_matRotation * tank->World());
		}
	}

	tank->Update();

	std::vector<GameModel *>::iterator iter = vGameModel.begin();
	while (iter != vGameModel.end())
	{
		bool isIntersectAABB = false;
		isIntersectAABB = Collision::IntersectRectToRect(*tank, *(*iter));

		if (isIntersectAABB)
			iter = vGameModel.erase(iter);
		else
		{
			(*iter)->Update();
			iter++;
		}
	}
}

void ExeHw::PreRender()
{
}

void ExeHw::Render()
{
	tank->Render();
	for (GameModel* gameModel : vGameModel)
		gameModel->Render();
}

void ExeHw::PostRender()
{
	
}

void ExeHw::ResizeScreen()
{
}

