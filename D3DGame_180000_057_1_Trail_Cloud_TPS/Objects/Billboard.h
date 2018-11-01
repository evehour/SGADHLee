#pragma once
#include "GameModel.h"

class Billboard : public GameModel
{
public:
	Billboard(ExecuteValues* values, wstring shaderFile, wstring diffuseFile);
	~Billboard();

	void FixedY(bool val) { bFixedY = val; }

	void Update();
	void Render();

private:
	Shader* billShader;
	ExecuteValues* values;

	RasterizerState* cullMode[2];

	bool bFixedY;
};