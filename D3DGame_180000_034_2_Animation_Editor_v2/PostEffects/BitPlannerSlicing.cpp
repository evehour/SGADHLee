#include "stdafx.h"
#include "BitPlannerSlicing.h"

BitPlannerSlicing::BitPlannerSlicing(ExecuteValues * values)
	//: Render2D(values, Shaders + L"032_BitPlannerSlicing.hlsl")
	: Render2D(values, Shaders + L"032_BitPlannerSlicing.hlsl")
{
	buffer = new Buffer();
}

BitPlannerSlicing::~BitPlannerSlicing()
{
	SAFE_DELETE(buffer);
}

void BitPlannerSlicing::Render()
{
	ImGui::Separator();
	ImGui::SliderInt("BitPlannerSlicing Select", &buffer->Data.ShiftCount, 0, 8);
	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
