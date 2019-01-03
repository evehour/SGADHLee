#include "Framework.h"
#include "ShadowMap.h"

ShadowMap* ShadowMap::instance = NULL;

ShadowMap::ShadowMap()
	: srv(NULL), rtv(NULL)
{
}

ShadowMap::~ShadowMap()
{
	SAFE_DELETE(viewport);
	SAFE_RELEASE(srv);
	SAFE_DELETE(rtv);
}

ShadowMap * ShadowMap::Get()
{
	return instance;
}

void ShadowMap::Create()
{
	assert(instance == NULL);

	instance = new ShadowMap();
	instance->Initialize();
}

void ShadowMap::Delete()
{
	SAFE_DELETE(instance);
}

void ShadowMap::Set(bool bClear, D3DXCOLOR clear)
{
	viewport->RSSetViewport();

	if (bClear == true)
	{
		D3D::GetDC()->ClearRenderTargetView(D3D::Get()->GetRenderTargetView(), clear);
		D3D::GetDC()->ClearRenderTargetView(rtv->RTV(), clear);
		D3D::GetDC()->ClearDepthStencilView(rtv->DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	ID3D11RenderTargetView* rtvs[1] = { rtv->RTV() };
	D3D::GetDC()->OMSetRenderTargets(1, rtvs, rtv->DSV());
}

void ShadowMap::Initialize()
{
	rtv = new RenderTargetView(4096, 4096, DXGI_FORMAT_R32G32B32A32_FLOAT);
	viewport = new Viewport(4096.0f, 4096.0f);
}
