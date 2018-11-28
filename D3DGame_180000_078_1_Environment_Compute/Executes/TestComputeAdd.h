#pragma once
#include "Execute.h"

class TestComputeAdd : public Execute
{
public:
	TestComputeAdd();
	~TestComputeAdd();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	void ExecuteCs();

private:
	ID3D11ShaderResourceView* srv1;
	ID3D11ShaderResourceView* srv2;
	ID3D11UnorderedAccessView* uav;

	ID3D11Buffer* outputBuffer;
	ID3D11Buffer* outputDebugBuffer;
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