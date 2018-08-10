#pragma once
#include "Execute.h"

class ExeHW1 : public Execute
{
private:
	class Cube* m_cube;
	class Terrain* m_terrain;

	D3DXMATRIX matCam, matCenter;
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 initCamPos;

public:
	ExeHW1(ExecuteValues* values);
	~ExeHW1();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	int fillModeNumber;
	ID3D11RasterizerState* fillMode[2];

	int cameraMode;
};