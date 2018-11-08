#pragma once

class BlendState
{
public:
	BlendState();
	~BlendState();

	void AlphaToCoverageEnable(bool val);
	void BlendEnable(bool val);
	void OMSetBlendState();

	void BlendOP(D3D11_BLEND_OP val);
	void DestBlend(D3D11_BLEND val);
	void SrcBlend(D3D11_BLEND val);
	void DestAlpha(D3D11_BLEND val);
	void SrcAlpha(D3D11_BLEND val);

private:
	void Changed();

private:
	D3D11_BLEND_DESC desc;
	ID3D11BlendState* state;
};