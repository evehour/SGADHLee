#pragma once
#include "Execute.h"

class ExeTankGame : public Execute
{
public:
	ExeTankGame(ExecuteValues* values);
	~ExeTankGame();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class GameTank* tank;
	vector<class GameBullet *> bullets;

	vector<class GameModel *> enemys;
};