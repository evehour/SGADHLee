#include "stdafx.h"
#include "ExeTankGame.h"
#include "../Objects/GameTank.h"
#include "../Objects/GameBullet.h"
#include "../Objects/MeshObject.h"
#include "../Landscape/Terrain.h"
#include "Physics/Collision.h"

ExeTankGame::ExeTankGame(ExecuteValues * values)
	: Execute(values)
{
	tank = new GameTank(Models + L"/Tank/", L"Tank.material");
	frotY = 0.0f;
	D3DXMatrixIdentity(&rotY);
}

ExeTankGame::ExeTankGame(ExecuteValues * values, Terrain * terrain)
	: Execute(values)
	, terrain(terrain)
{
	tank = new GameTank(Models + L"/Tank/", L"Tank.material");
	//tank->Position(D3DXVECTOR3(99, 0, 16));
	tank->Position(D3DXVECTOR3(5, 0, 5));
	frotY = 0.0f;
	D3DXMatrixIdentity(&rotY);
	D3DXMatrixScaling(&scale, 0.12f, 0.1f, 0.1f);

	for (int i = 0; i < 30; i++)
	{
		MeshObject* enemy;
		
		if (i % 2 == 0)
		{
			enemy = new MeshObject
			(
				Materials + L"Meshes/", L"Cylinder.material",
				Models + L"Meshes/", L"Cylinder.mesh"
			);
		}
		else
		{
			enemy = new MeshObject
			(
				Materials + L"Meshes/", L"Teapot.material",
				Models + L"Meshes/", L"Teapot.mesh"
			);
		}

		D3DXVECTOR3 _pos;
		_pos = D3DXVECTOR3(
			Math::Random(0.0f, (float)(128 * 3)),
			0.0f,
			Math::Random(0.0f, (float)(128 * 3)));

		_pos.y = terrain->Y(_pos);
		enemy->Position(_pos);

		enemys.push_back(enemy);
	}
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
#if false
	D3DXMATRIX rotX, _rotY, rotZ;
	D3DXMATRIX R, T;
	D3DXVECTOR3 _dir = D3DXVECTOR3(0, 0, 1) * 10.0f * Time::Get()->Delta();
	D3DXVECTOR3 _v[4];
	float _mapY = 0.0f;

	std::vector<D3DXVECTOR3> boundBox;
	tank->GetBoundSpace(boundBox);

	D3DXMatrixIdentity(&R);
	D3DXMatrixIdentity(&rotX);
	D3DXMatrixIdentity(&_rotY);
	D3DXMatrixIdentity(&rotZ);
	D3DXMatrixIdentity(&T);
	///////////////////////////////////////////////////////////////////////////
	// 차체 제어
	if (!Mouse::Get()->Press(1))
	{
		if (Keyboard::Get()->Press('A'))
		{
			D3DXMatrixRotationY(&_rotY, Math::ToRadian(-100.0f) * Time::Get()->Delta());
		}
		else if (Keyboard::Get()->Press('D'))
		{
			D3DXMatrixRotationY(&_rotY, Math::ToRadian(100.0f) * Time::Get()->Delta());
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

	{
		// HeightMap
		_v[0] = boundBox[4]; // Front Left
		_v[1] = boundBox[6]; // Front Right
		_v[2] = boundBox[0]; // Back Left
		_v[3] = boundBox[2]; // Back Right
		D3DXMATRIX deltaYMatrix;
		D3DXMATRIX deltaMatrix;
		deltaMatrix = (rotY * T) * tank->World();
		_mapY = terrain->Y(D3DXVECTOR3(deltaMatrix._41, deltaMatrix._42, deltaMatrix._43));
		T._42 = _mapY;
		for (int i = 0; i < 4; i++)
		{
			D3DXVec3TransformCoord(&_v[i], &_v[i], &deltaMatrix);
			_v[i].y = terrain->Y(_v[i]);
		}

		D3DXVECTOR3 _vAxis;
		D3DXVECTOR3 widthDir, lengthDir;

		if (_v[0].y > _v[2].y)
		{
			D3DXVec3Normalize(&lengthDir, &(_v[0] - _v[2]));
			_vAxis.x = lengthDir.y;
		}
		else
		{
			D3DXVec3Normalize(&lengthDir, &(_v[2] - _v[0]));
			_vAxis.x = -lengthDir.y;
		}

		if (_v[1].y > _v[0].y)
		{
			D3DXVec3Normalize(&widthDir, &(_v[1] - _v[0]));
			_vAxis.z = -widthDir.y;
		}
		else
		{
			D3DXVec3Normalize(&widthDir, &(_v[0] - _v[1]));
			_vAxis.z = widthDir.y;
		}
		D3DXMatrixRotationX(&rotX, _vAxis.x);
		D3DXMatrixRotationZ(&rotZ, _vAxis.z);
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

	R = rotX * _rotY * rotZ;
#if true
	D3DXMATRIX dTWorld;
	D3DXMatrixTranslation(&dTWorld, tank->World()._41, 0, tank->World()._43);
	dTWorld = rotY * dTWorld;
	rotY = _rotY * rotY;
	tank->World((R * T) * dTWorld);
#else
	tank->World((R * T) * tank->World());
#endif
	///////////////////////////////////////////////////////////////////////////
#else

#if true
	D3DXMATRIX S = scale, R, T;
	D3DXMATRIX rotX, _rotY, rotZ;
	D3DXMATRIX deltaMatrix;

	D3DXVECTOR3 _pos(0, 0, 0), _dir(0, 0, 0);
	D3DXVECTOR3 _v[4];

	std::vector<D3DXVECTOR3> boundBox;
	tank->GetBoundSpace(boundBox);

	D3DXMatrixIdentity(&R);
	D3DXMatrixIdentity(&T);
	D3DXMatrixIdentity(&rotX);
	D3DXMatrixIdentity(&_rotY);
	D3DXMatrixIdentity(&rotZ);

	_pos = tank->Position();

	if (!Mouse::Get()->Press(1))
	{
		if (Keyboard::Get()->Press('A'))
		{
			frotY += Math::ToRadian(-100.0f) * Time::Get()->Delta();
			if (frotY < 0) frotY += 2 * Math::PI;
			D3DXMatrixRotationY(&_rotY, Math::ToRadian(-100.0f) * Time::Get()->Delta());
		}
		else if (Keyboard::Get()->Press('D'))
		{
			frotY += Math::ToRadian(100.0f) * Time::Get()->Delta();
			if (frotY > (2 * Math::PI)) frotY -= 2 * Math::PI;
			D3DXMatrixRotationY(&_rotY, Math::ToRadian(100.0f) * Time::Get()->Delta());
		}

		if (Keyboard::Get()->Press('W'))
		{
			tank->SetTankState(TANK_STATE_MOVE);
			tank->SetWheelDir(true);
			_dir = D3DXVECTOR3(0, 0, -1) * 10.0f * Time::Delta();
		}
		else if (Keyboard::Get()->Press('S'))
		{
			tank->SetTankState(TANK_STATE_MOVE);
			tank->SetWheelDir(false);
			_dir = D3DXVECTOR3(0, 0, 1) * 10.0f * Time::Delta();
		}

		if (Keyboard::Get()->Up('W') || Keyboard::Get()->Up('S'))
		{
			tank->SetTankState(TANK_STATE_IDLE);
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
	}

	{
		//D3DXMatrixRotationY(&rotY, frotY);
		rotY = _rotY * rotY;
	}

	D3DXVec3TransformNormal(&_dir, &_dir, &rotY);
	_pos += _dir;
	_pos.y = terrain->Y(D3DXVECTOR3(_pos.x, 0, _pos.z));
	D3DXMatrixTranslation(&T, _pos.x, _pos.y, _pos.z);

	D3DXVECTOR3 temp;
	temp = -tank->Direction();

	D3DXVECTOR3 _vAxis(0, 0, 0);
	{
		// HeightMap
		_v[0] = boundBox[4]; // Front Left
		_v[1] = boundBox[6]; // Front Right
		_v[2] = boundBox[0]; // Near Left
		_v[3] = boundBox[2]; // Near Right

		deltaMatrix = S * rotY * T;

		for (int i = 0; i < 4; i++)
		{
			D3DXVec3TransformCoord(&_v[i], &_v[i], &tank->World());
			_v[i].y = terrain->Y(_v[i]);
		}

		D3DXVECTOR3 widthDir, lengthDir;

		if (_v[0].y > _v[2].y)
		{
			D3DXVec3Normalize(&lengthDir, &(_v[0] - _v[2]));
			_vAxis.x = -lengthDir.y;
		}
		else
		{
			D3DXVec3Normalize(&lengthDir, &(_v[2] - _v[0]));
			_vAxis.x = lengthDir.y;
		}

		if (_v[1].y > _v[0].y)
		{
			D3DXVec3Normalize(&widthDir, &(_v[0] - _v[1]));
			_vAxis.z = -widthDir.y;
		}
		else
		{
			D3DXVec3Normalize(&widthDir, &(_v[1] - _v[0]));
			_vAxis.z = widthDir.y;
		}
		_vAxis.y = frotY;
		D3DXMatrixRotationX(&rotX, _vAxis.x);
		D3DXMatrixRotationZ(&rotZ, _vAxis.z);
	}

	R = rotZ * rotX * rotY;
	D3DXMatrixRotationYawPitchRoll(&R, _vAxis.y, _vAxis.x, _vAxis.z);
	tank->World(S * R * T);
#else
	D3DXMATRIX S = scale, R, T;
	D3DXMATRIX rotX, _rotY, rotZ;
	D3DXVECTOR3 _pos(0, 0, 0), _dir(0, 0, 0);

	D3DXMatrixIdentity(&T);
	D3DXMatrixIdentity(&rotX);
	D3DXMatrixIdentity(&_rotY);
	D3DXMatrixIdentity(&rotZ);

	_pos = tank->Position();

	if (!Mouse::Get()->Press(1))
	{
		if (Keyboard::Get()->Press('A'))
		{
			frotY += Math::ToRadian(-100.0f) * Time::Get()->Delta();
			//D3DXMatrixRotationY(&_rotY, Math::ToRadian(-100.0f) * Time::Get()->Delta());
		}
		else if (Keyboard::Get()->Press('D'))
		{
			frotY += Math::ToRadian(100.0f) * Time::Get()->Delta();
			//D3DXMatrixRotationY(&_rotY, Math::ToRadian(100.0f) * Time::Get()->Delta());
		}

		if (Keyboard::Get()->Press('W'))
		{
			tank->SetTankState(TANK_STATE_MOVE);
			tank->SetWheelDir(true);
			_dir = D3DXVECTOR3(0, 0, -1) * 10.0f * Time::Delta();
		}
		else if (Keyboard::Get()->Press('S'))
		{
			tank->SetTankState(TANK_STATE_MOVE);
			tank->SetWheelDir(false);
			_dir = D3DXVECTOR3(0, 0, 1) * 10.0f * Time::Delta();
		}

		if (Keyboard::Get()->Up('W') || Keyboard::Get()->Up('S'))
		{
			tank->SetTankState(TANK_STATE_IDLE);
		}
	}

	rotY = _rotY * rotY;
	D3DXVec3TransformNormal(&_dir, &_dir, &rotY);
	_pos += _dir;
	_pos.y = terrain->Y(D3DXVECTOR3(_pos.x, 0, _pos.z));
	D3DXMatrixTranslation(&T, _pos.x, _pos.y, _pos.z);

	D3DXVECTOR3 _vSize;
	float rad = 0.0f;
	tank->BoundSize(_vSize);
	tank->Radius(rad);

	D3DXVec3TransformCoord(&_vSize, &_vSize, &S);

	D3DXVECTOR3 _v[4];
#if true
	_v[0] = _pos + (tank->Direction() * (_vSize.z / 2.0f)); // Front
	_v[1] = _pos - (tank->Direction() * (_vSize.z / 2.0f)); // Near
	_v[2] = _pos - (tank->Right() * (_vSize.x / 2.0f)); // Left
	_v[3] = _pos + (tank->Right() * (_vSize.x / 2.0f)); // Right

	for (int i = 0; i < 4; i++)
		_v[i].y = terrain->Y(_v[i]);

#else
	_v[0] =  (D3DXVECTOR3(0.0f, 0.0f, 1.0f) * (_vSize.z / 2.0f)); // Front
	_v[1] = -(D3DXVECTOR3(0.0f, 0.0f, 1.0f) * (_vSize.z / 2.0f)); // Near
	_v[2] = -(D3DXVECTOR3(1.0f, 0.0f, 0.0f) * (_vSize.x / 2.0f)); // Left
	_v[3] =  (D3DXVECTOR3(1.0f, 0.0f, 0.0f) * (_vSize.x / 2.0f)); // Right
	for (int i = 0; i < 4; i++)
	{
		D3DXVec3TransformNormal(&_v[i], &_v[i], &rotY);
		_v[i] = _pos + _v[i];
		_v[i].y = terrain->Y(_v[i]);
	}
#endif
	_v[1].x = _v[0].x;
	_v[2].z = _v[3].z;

	D3DXVECTOR3 axis;
	D3DXVECTOR3 temp;
	if (_v[0].y > _v[1].y)
	{
		D3DXVec3Normalize(&temp, &(_v[0] - _v[1]));
		axis.x = -temp.y;
	}
	else
	{
		D3DXVec3Normalize(&temp, &(_v[1] - _v[0]));
		axis.x = temp.y;
	}
	if (_v[3] > _v[2])
	{
		D3DXVec3Normalize(&temp, &(_v[3] - _v[2]));
		axis.z = temp.y;
	}
	else
	{
		D3DXVec3Normalize(&temp, &(_v[2] - _v[3]));
		axis.z = -temp.y;
	}
	if (-tank->Direction().z < 0)
	{
		axis = -axis;
	}
	axis.y = frotY;

	D3DXMatrixRotationX(&rotX, axis.x);
	D3DXMatrixRotationY(&rotY, axis.y);
	D3DXMatrixRotationZ(&rotZ, axis.z);
	R = rotX * rotY * rotZ;
	tank->World(S * R * T);
#endif
#endif

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

	if(false){
		D3DXVECTOR3 k = tank->Position() + D3DXVECTOR3(0.0f, 5.0f, -15.0f);
		values->MainCamera->Position(k.x, k.y, k.z);
	}

	vector<GameBullet *>::iterator iterBullets;
	iterBullets = bullets.begin();
	while (iterBullets != bullets.end())
	{
		vector<MeshObject *>::iterator iterEnemys;
		bool canDel = false;
		for (iterEnemys = enemys.begin(); iterEnemys != enemys.end();)
		{
			bool b = Collision::IntersectRectToRect(
				*(GameModel*)(*iterBullets),
				*(GameModel*)(*iterEnemys)
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

	for (MeshObject* enemy : enemys)
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
				MeshObject* p = new MeshObject(
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
				MeshObject* p = new MeshObject(
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

