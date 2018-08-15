#pragma once
#include "GameModel.h"

class GameBullet : public GameModel
{
public:
	GameBullet(wstring matFolder, wstring matFile);
	~GameBullet();

	void Update() override;
	void Render() override;

private:
	D3DXMATRIX launchMatrix;

public:
	void SetLaunchMatrix(D3DXMATRIX& matrix);
};