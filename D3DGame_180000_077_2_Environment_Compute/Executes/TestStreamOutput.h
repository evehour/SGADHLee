#pragma once
#include "Execute.h"

class TestStreamOutput : public Execute
{
public:
	TestStreamOutput();
	~TestStreamOutput();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	ID3D11Buffer* initBuffer;
	ID3D11Buffer* drawVBuffer;
	ID3D11Buffer* SOVBuffer;

	Shader *shader;

private:
	bool bFirstRender;
};