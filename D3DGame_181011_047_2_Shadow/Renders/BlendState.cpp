#include "stdafx.h"
#include "BlendState.h"

BlendState::BlendState()
	: state(NULL)
{
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));

	desc.AlphaToCoverageEnable = false; // 외곽선 처리를 어떻게 할지 결정
	desc.IndependentBlendEnable = false; // 멀티 랜더링 용 변수

	desc.RenderTarget[0].BlendEnable = false;

	// Dc * Db (OP) Sc * Sb
	// Dc * (1-a) + Sc * (a)
	// 옵션의 SRC는 SrcBlend를 말함.
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	// 깊이 테스트 할 때 이야기 할꺼임
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	Changed();
}

BlendState::~BlendState()
{
	SAFE_RELEASE(state);
}

void BlendState::AlphaToCoverageEnable(bool val)
{
	desc.AlphaToCoverageEnable = val;

	Changed();
}

void BlendState::BlendEnable(bool val)
{
	desc.RenderTarget[0].BlendEnable = val;

	Changed();
}

void BlendState::OMSetBlendState()
{
	D3D::GetDC()->OMSetBlendState(state, NULL, 0xFF);
}

void BlendState::BlendOP(D3D11_BLEND_OP val)
{
	desc.RenderTarget[0].BlendOp = val;

	Changed();
}

void BlendState::DestBlend(D3D11_BLEND val)
{
	desc.RenderTarget[0].DestBlend = val;

	Changed();
}

void BlendState::SrcBlend(D3D11_BLEND val)
{
	desc.RenderTarget[0].SrcBlend = val;

	Changed();
}

void BlendState::Changed()
{
	SAFE_RELEASE(state);

	HRESULT hr = D3D::GetDevice()->CreateBlendState(&desc, &state);
	assert(SUCCEEDED(hr));
}
