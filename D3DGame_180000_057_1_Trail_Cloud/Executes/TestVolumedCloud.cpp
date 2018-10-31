#include "stdafx.h"
#include "TestVolumedCloud.h"

#include "../Landscape/VolumedCloud.h"

TestVolumedCloud::TestVolumedCloud(ExecuteValues * values)
	: Execute(values)
{
	volumedCloud = new VolumedCloud(Shaders + L"047_VolumeInstanceCloud.hlsl", Textures + L"CloudSpriteSheetOrg.png", VolumedCloud::SkyType::SpotClouds, values);
}

TestVolumedCloud::~TestVolumedCloud()
{

}

void TestVolumedCloud::Update()
{
	volumedCloud->Update();
}

void TestVolumedCloud::PreRender()
{

}

void TestVolumedCloud::Render()
{
	volumedCloud->Render();
}

void TestVolumedCloud::PostRender()
{

}
