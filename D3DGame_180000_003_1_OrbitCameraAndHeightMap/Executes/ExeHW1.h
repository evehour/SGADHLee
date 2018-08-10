#pragma once
#include "Execute.h"

class ExeHW1 : public Execute
{
private:
	class Sphere* m_sphere;
	class Terrain* m_terrain;

public:
	ExeHW1(ExecuteValues* values);
	~ExeHW1();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	D3DXVECTOR3 beforeCamPos;
	D3DXVECTOR2 dgr;
	float degree;
	float radius;

	int fillModeNumber;
	ID3D11RasterizerState* fillMode[2];
};