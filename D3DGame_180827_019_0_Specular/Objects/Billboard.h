#pragma once
#include "GameModel.h"

class Billboard : public GameModel
{
public:
	Billboard(wstring shaderFile, wstring diffuseFile);
	~Billboard();

	void FixedY(bool val) { bFixedY = val; }

	void Update();
	void Render();

private:
	RasterizerState* cullMode[2];

	bool bFixedY;
};