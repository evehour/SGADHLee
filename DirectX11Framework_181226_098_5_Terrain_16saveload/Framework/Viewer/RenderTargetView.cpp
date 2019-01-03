#include "Framework.h"
#include "RenderTargetView.h"

RenderTargetView::RenderTargetView(UINT width, UINT height, DXGI_FORMAT format)
	: width(width), height(height), format(format)
{
	viewport = new Viewport((float)this->width, (float)this->height);
	//Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
		assert(SUCCEEDED(hr));
	}

	//Create RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = D3D::GetDevice()->CreateRenderTargetView(texture, &desc, &rtv);
		assert(SUCCEEDED(hr));
	}

	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &srv);
		assert(SUCCEEDED(hr));
	}

	//Create Texture - DSV¿ë
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &dsvTexture);
		assert(SUCCEEDED(hr));
	}

	//Create SRV - DSV¿ë
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;
		desc.Texture2D.MostDetailedMip = 0;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(dsvTexture, &desc, &dsrv);
		assert(SUCCEEDED(hr));
	}

	//Create DSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = D3D::GetDevice()->CreateDepthStencilView(dsvTexture, &desc, &dsv);
		assert(SUCCEEDED(hr));
	}

	//Create UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		uavDesc.Format = format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		HRESULT hr = D3D::GetDevice()->CreateUnorderedAccessView(texture, &uavDesc, &uav);
		assert(SUCCEEDED(hr));
	}
}

RenderTargetView::~RenderTargetView()
{
	SAFE_RELEASE(uav);
	SAFE_RELEASE(dsv);
	SAFE_RELEASE(dsrv);
	SAFE_RELEASE(rtv);
	SAFE_RELEASE(srv);
	SAFE_RELEASE(dsvTexture);
	SAFE_RELEASE(texture);
	SAFE_DELETE(viewport);
}

void RenderTargetView::Clear(D3DXCOLOR clear)
{
	D3D::Get()->Clear(clear, rtv, dsv);
}

Viewport * RenderTargetView::GetViewport()
{
	return viewport;
}
