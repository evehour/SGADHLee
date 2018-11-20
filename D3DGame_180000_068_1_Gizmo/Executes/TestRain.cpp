#include "stdafx.h"
#include "TestRain.h"
#include "../Landscape/NRain.h"
#include "../Draw/MeshSphere.h"
#include "../Draw/MeshGrid.h"
#include "../Renders/Material.h"


TestRain::TestRain(ExecuteValues * values)
	:Execute(values)
{
	rain = new NRain(values);

	material = new Material();
	surfaceShader = new Shader(Shaders + L"050_AdvancedRain.hlsl", "VSSurface", "PSSurface");
	material->SetShader(surfaceShader);
	material->SetDiffuseMap(Textures + L"Stones.png");
	material->SetNormalMap(Textures + L"Stones_normal.png");

	//grid = new MeshSphere(material, 3);
	grid = new MeshGrid(material, 1.0f, 1.0f, 10.0f, 10.0f);

	splash = new Texture(Textures + L"rainTextures/splashes/SDiffuseVolume.dds");
	splashNormal = new Texture(Textures + L"rainTextures/splashes/SBumpVolume.dds");

	// Light
	{
		lightBuffer = new LightBuffer();

		LightData data;
		{
			// Capsule Light
			LightDesc desc;
			desc.Type = ELightType::Capsule;
			desc.Position = D3DXVECTOR3(60, 10, 0);
			desc.Direction = D3DXVECTOR3(1, 0, 0);

			desc.Range = 30;
			desc.Length = 20;

			desc.Color = D3DXVECTOR3(1, 1, 1);

			data.Setting(0, desc);
		}
		{
			LightDesc desc;
			desc.Type = ELightType::Spot;
			desc.Position = D3DXVECTOR3(0, 10, 0);
			desc.Direction = D3DXVECTOR3(1, -1, 0);
			desc.Range = 50;
			//desc.Length = 80;
			desc.Outer = Math::ToRadian(65.0f);
			desc.Inner = Math::ToRadian(45.0f);
			desc.Color = D3DXVECTOR3(1, 1, 1);
			data.Setting(1, desc);
		}
		lightBuffer->AddLight(data);
	}
}

TestRain::~TestRain()
{
	SAFE_DELETE(lightBuffer);

	SAFE_DELETE(surfaceShader);
	SAFE_DELETE(material);
	SAFE_DELETE(grid);

	SAFE_DELETE(rain);

	SAFE_DELETE(splash);
	SAFE_DELETE(splashNormal);
}

void TestRain::Update()
{
	rain->Update();
}

void TestRain::PreRender()
{

}

void TestRain::Render()
{
	lightBuffer->SetPSBuffer(2);
	splash->SetShaderResource(12);
	splashNormal->SetShaderResource(13);
	grid->Render();

	rain->Render();
}

void TestRain::PostRender()
{

}


