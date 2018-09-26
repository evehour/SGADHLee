#pragma once
#include "Execute.h"

class TestVersus : public Execute
{
public:
	TestVersus(ExecuteValues* values);
	~TestVersus();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen(){}

private:
	Shader* shader;
	class Player* player;

	UINT monsterCount;
	class Monster** monster;

private:
	void PlayerController();
	void MonsterController();

private:
};