#pragma once
#include "GameModel.h"

class Billboard : public GameModel
{
public:
	Billboard(ExecuteValues* values, wstring shaderFile, wstring diffuseFile);
	~Billboard();

	void Update();
	void Render();

private:
	ExecuteValues* values;

	bool bFixedY;
};