#pragma once
#include "Execute.h"

//#define __USING_PATHFINDING__

class TestTerrain : public Execute
{
public:
	TestTerrain();
	~TestTerrain();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void PostRender2D() {};
	void ResizeScreen() {};

private:
	class CubeSky* sky;
	class Terrain* terrain;
	class PathFinding* pathFinding;

private:
	bool bUsingPathFinding;
	bool picked;
	D3DXVECTOR3 pickPixel;
	D3DXVECTOR3 pickWorldPosition;
	D3DXVECTOR3 pickMapTileWorldPos;
};