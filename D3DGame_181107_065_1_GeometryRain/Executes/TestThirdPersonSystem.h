#pragma once
#include "Execute.h"

class TestThirdPersonSystem : public Execute
{
public:
	TestThirdPersonSystem(ExecuteValues* values);
	~TestThirdPersonSystem();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen(){}

private:
	class MeshPlane* plane;
	Shader* objectShader;

private:
	Shader* modelShader;
	class Player* player;

	UINT monsterCount;
	class Monster** monster;
	UINT idx;

private:
	void PlayerController();
	void MonsterController();

private:
	bool rayCollision;
	class LineMake* rayLine;

	class OrbitCamera* MainCamera;
};