#pragma once

class ShadowMap
{
public:
	static ShadowMap* Get();

	static void Create();
	static void Delete();

	void Set(bool bClear = false, D3DXCOLOR clear = 0xFF000000);

	ID3D11RenderTargetView* GetRTV() { return rtv->RTV(); }
	ID3D11DepthStencilView* GetDSV() { return rtv->DSV(); }
	ID3D11ShaderResourceView* SRV() { return rtv->SRV(); }
	ID3D11ShaderResourceView* DSRV() { return rtv->DSRV(); }

private:
	ShadowMap();
	~ShadowMap();

	void Initialize();

private:
	static ShadowMap* instance;

	class RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;
	class Viewport* viewport;
};

#define Shadowmap ShadowMap::Get()