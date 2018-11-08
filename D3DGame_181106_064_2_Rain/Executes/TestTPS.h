#pragma once
#include "Execute.h"

class TestTPS : public Execute
{
public:
	TestTPS(ExecuteValues* values);
	~TestTPS();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen(){}

private:
	class MeshPlane* plane;
	Shader* objectShader;

	class Shadow* shadow;

private:
	Shader* modelShader;
	class Player* player;

	UINT monsterCount;
	class Monster** monster;

private:
	void PlayerController();
	void MonsterController();

private:
};