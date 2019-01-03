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
	D3D::Get()->SetRenderTarget(rtv->RTV(), rtv->DSV());

	if(bClear == true)
		D3D::Get()->Clear(clear, rtv->RTV(), rtv->DSV());

	viewport->RSSetViewport();
}

void ShadowMap::Initialize()
{
	rtv = new RenderTargetView(2048, 2048, DXGI_FORMAT_R32_FLOAT);
	viewport = new Viewport(2048.0f, 2048.0f);
}
