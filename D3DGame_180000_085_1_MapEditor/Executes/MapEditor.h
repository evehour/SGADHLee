#pragma once
#include "Execute.h"

class GameModel;
class GameAnimModel;

class MapEditor : public Execute
{
public:
	MapEditor();
	~MapEditor();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void PostRender2D();
	void ResizeScreen() {}

private:
	GameAnimModel* ma;
};