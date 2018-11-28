#pragma once
#include "Execute.h"

class TestComputeAppend : public Execute
{
public:
	TestComputeAppend();
	~TestComputeAppend();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	void ExecuteCs();

private:
	ID3D11ShaderResourceView* consume;
	ID3D11UnorderedAccessView* append;

	ID3D11Buffer* outputBuffer;
	ID3D11Buffer* outputDebugBuffer;
private:
	UINT dataSize;

	Shader* shader;

private:
	struct Particle
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Velocity;
		D3DXVECTOR3 Accelation;
	};
};