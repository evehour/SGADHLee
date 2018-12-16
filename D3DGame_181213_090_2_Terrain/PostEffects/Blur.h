#pragma once

class Blur
{
public:
	Blur(wstring shaderFile, UINT width, UINT height, DXGI_FORMAT format, int blurCount);
	~Blur();

	void ChangeBlurCount(int blurCount, float sigma);
	void SetWeights(float sigma);
	void Dispatch(ID3D11ShaderResourceView* inputSRV, ID3D11UnorderedAccessView* inputUAV);

private:
	int blurCount;

	Shader* shader;
	UINT width, height;

	// ������ �� SRV�� �Ѿ�� �༮
	ID3D11ShaderResourceView* srv;
	ID3D11UnorderedAccessView* uav;
};