#pragma once
#include "Execute.h"

class TestExecute : public Execute
{
public:
	TestExecute(ExecuteValues* values);
	~TestExecute();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	ID3D11Buffer* initBuffer;
	ID3D11Buffer* drawVBuffer;
	ID3D11Buffer* SOVBuffer;

	Shader *shader1, *shader2;

	DepthStencilState* depthStencilState[2];

private:
	bool bFirstRender;
};