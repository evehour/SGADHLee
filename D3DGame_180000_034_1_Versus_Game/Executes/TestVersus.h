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
	class Monster* monster;

	class ModelClip* playerClips[1000];
	class ModelClip* monsterClips[1000];

private:
	void PlayerController();
	void MonsterController();

private:
};