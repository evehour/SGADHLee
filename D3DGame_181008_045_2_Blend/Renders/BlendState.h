#pragma once

class BlendState
{
public:
	BlendState();
	~BlendState();

	void AlphaToCoverageEnable(bool val);
	void IndependentBlendEnable(bool val);

	void BlendEnable(bool val);
	void OMSetBlendState();

private:
	void Changed();

private:
	D3D11_BLEND_DESC desc;
	ID3D11BlendState* state;
};