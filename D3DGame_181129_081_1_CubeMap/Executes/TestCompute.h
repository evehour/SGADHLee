#pragma once
#include "Execute.h"

class TestCompute : public Execute
{
public:
	TestCompute();
	~TestCompute();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	void ExecuteCs();

private:
	CsResource* input[2];
	CsResource* output;

private:
	UINT dataSize;

	Shader* shader;

private:
	struct Data
	{
		D3DXVECTOR3 V1;
		D3DXVECTOR2 V2;
	};
};