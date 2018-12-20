#pragma once

class RenderTargetCube
{
public:
	RenderTargetCube(UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RenderTargetCube();

	ID3D11Texture2D* Texture() { return texture; }
	ID3D11ShaderResourceView* SRV() { return srv; }
	ID3D11RenderTargetView* RTV() { return rtv; }

private:
	UINT width, height;
	DXGI_FORMAT format;

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	ID3D11RenderTargetView* rtv;
};