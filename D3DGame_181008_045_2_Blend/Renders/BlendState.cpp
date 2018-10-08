#include "stdafx.h"
#include "BlendState.h"

BlendState::BlendState()
	: state(NULL)
{
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));

	desc.AlphaToCoverageEnable = false; // �ܰ��� ó���� ��� ���� ����
	desc.IndependentBlendEnable = false; // ��Ƽ ������ �� ����

	desc.RenderTarget[0].BlendEnable = false;

	// Dc * Db (OP) Sc * Sb
	// Dc * (1-a) + Sc * (a)
	// �ɼ��� SRC�� SrcBlend�� ����.
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	// ���� �׽�Ʈ �� �� �̾߱� �Ҳ���
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

void BlendState::Changed()
{
	SAFE_RELEASE(state);

	HRESULT hr = D3D::GetDevice()->CreateBlendState(&desc, &state);
	assert(SUCCEEDED(hr));
}
