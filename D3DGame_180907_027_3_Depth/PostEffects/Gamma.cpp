#include "stdafx.h"
#include "Gamma.h"

Gamma::Gamma(ExecuteValues * values)
	: Render2D(values, Shaders + L"032_Gamma.hlsl")
{
	buffer = new Buffer();
}

Gamma::~Gamma()
{
	SAFE_DELETE(buffer);
}

void Gamma::Render()
{
	ImGui::Separator();
	ImGui::SliderFloat("Gamma Level", &buffer->Data.GammaLevel, 0.000f, 20.000f);
	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
