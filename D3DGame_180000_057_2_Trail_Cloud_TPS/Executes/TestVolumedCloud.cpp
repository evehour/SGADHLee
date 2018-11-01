#include "stdafx.h"
#include "TestVolumedCloud.h"

#include "../Landscape/VolumedCloud.h"

TestVolumedCloud::TestVolumedCloud(ExecuteValues * values)
	: Execute(values)
	, selectCloud(0)
{
	for (UINT i = 0; i < 4; i++)
	{
		volumedCloud[i] = new VolumedCloud(Shaders + L"047_VolumeInstanceCloud.hlsl", Textures + L"CloudSpriteSheetOrg.png", (VolumedCloud::SkyType)i, values);
	}
	
	cloudColor = { 1, 1, 1, 1 };
}

TestVolumedCloud::~TestVolumedCloud()
{
	for (UINT i = 0;i < 4;i++)
	{
		SAFE_DELETE(volumedCloud[i])
	}
}

void TestVolumedCloud::Update()
{
	volumedCloud[selectCloud]->Update();
}

void TestVolumedCloud::PreRender()
{

}

void TestVolumedCloud::Render()
{
	volumedCloud[selectCloud]->Render();

	ImGui::Begin("Volumed Cloud");
	{
		ImGui::SliderInt("Cloud Type", &selectCloud, 0, 3);
		ImGui::ColorEdit4("Cloud Color", (float*)&cloudColor, ImGuiColorEditFlags_Float);
		volumedCloud[selectCloud]->SetCloudColor(cloudColor);
	}
	ImGui::End();
}

void TestVolumedCloud::PostRender()
{

}
