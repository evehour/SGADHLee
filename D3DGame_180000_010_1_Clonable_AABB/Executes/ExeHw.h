#pragma once
#include "Execute.h"

class GameModel;

class ExeHw : public Execute
{
public:
	ExeHw(ExecuteValues* values);
	~ExeHw();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	GameModel* tank;

	std::vector<GameModel *> vGameModel;
};