#include "stdafx.h"
#include "GaussianBlur.h"

GaussianBlur::GaussianBlur(ExecuteValues * values)
	: Render2D(values, Shaders + L"032_GaussianBlur.hlsl")
{
	buffer = new Buffer();
}

GaussianBlur::~GaussianBlur()
{
	SAFE_DELETE(buffer);
}

void GaussianBlur::Render()
{
	ImGui::Separator();
	ImGui::SliderInt("GaussianBlur Count", &buffer->Data.Count, 0, 20);
	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
