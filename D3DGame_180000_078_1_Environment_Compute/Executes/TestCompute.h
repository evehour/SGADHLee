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
	Shader* shader;
	vector<UINT> buffer, chkval;

	UINT NUM_COUNT;
private:
	const UINT BLOCK_SIZE = 32;
	const UINT NUM_ELEMENTS = BLOCK_SIZE * BLOCK_SIZE;
	const UINT MATRIX_WIDTH = BLOCK_SIZE;
	const UINT MATRIX_HEIGHT = NUM_ELEMENTS / BLOCK_SIZE;
	const UINT TRANSPOSE_BLOCK_SIZE = 16;

	ID3D11Buffer*               pBuffer1 = nullptr;
	ID3D11ShaderResourceView*   pBuffer1SRV = nullptr;
	ID3D11UnorderedAccessView*  pBuffer1UAV = nullptr;
	ID3D11Buffer*               pBuffer2 = nullptr;
	ID3D11ShaderResourceView*   pBuffer2SRV = nullptr;
	ID3D11UnorderedAccessView*  pBuffer2UAV = nullptr;

	ID3D11Buffer*               pReadBackBuffer = nullptr;
};