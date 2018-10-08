#include "stdafx.h"
#include "Mosaic.h"

Mosaic::Mosaic(ExecuteValues * values)
	: Render2D(values, Shaders + L"032_Mosaic.hlsl")
{
	buffer = new Buffer();
}

Mosaic::~Mosaic()
{
	SAFE_DELETE(buffer);
}

void Mosaic::Render()
{
	ImGui::Separator();
	ImGui::SliderFloat("Mosaic Level", &buffer->Data.MosaicLevel, 0.0000f, 1.0000f);
	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
