#include "stdafx.h"
#include "TestAmbient.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshCube.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshBunny.h"

TestAmbient::TestAmbient(ExecuteValues * values)
	: Execute(values)
{
	shader = new Shader(Shaders + L"046_Lighting.hlsl");

	plane = new MeshPlane();
	plane->Scale(10, 1, 10);
	plane->SetShader(shader);
	plane->SetDiffuse(1, 1, 1);

	cube = new MeshCube();
	cube->Scale(3, 3, 3);
	cube->Position(-10, 1.5f, 0);
	cube->SetShader(shader);
	cube->SetDiffuse(0, 0.7f, 0);

	sphere = new MeshSphere();
	sphere->Scale(3, 3, 3);
	sphere->Position(10, 1.5f, 0);
	sphere->SetShader(shader);
	sphere->SetDiffuse(0, 0, 1);

	sphere2 = new MeshSphere();
	sphere2->Scale(3, 3, 3);
	sphere2->Position(10, 1.5f, 10);
	sphere2->SetShader(shader);
	sphere2->SetDiffuse(1, 1, 1);

	bunny = new MeshBunny();
	bunny->Scale(0.02f, 0.02f, 0.02f);
	bunny->Position(0, 5.0f, 0);
	bunny->SetShader(shader);
	bunny->SetDiffuse(1, 1, 1);
	bunny->SetDiffuseMap(Textures + L"White.png");
	bunny->SetShininess(20);

	lightBuffer = new LightBuffer();

	LightData data;
	{
		// Capsule Light
		LightDesc desc;
		desc.Type = ELightType::Capsule;
		desc.Position = D3DXVECTOR3(0, 10, 0);
		desc.Direction = D3DXVECTOR3(1, 0, 0);

		desc.Range = 30;
		desc.Length = 20;

		desc.Color = D3DXVECTOR3(1, 0, 0);

		data.Setting(0, desc);
		//data.Setting(1, desc);
		//data.Setting(2, desc);
		//data.Setting(3, desc);
	}
	lightBuffer->AddLight(data);
}

TestAmbient::~TestAmbient()
{
	SAFE_DELETE(bunny);
	SAFE_DELETE(sphere2);
	SAFE_DELETE(sphere);
	SAFE_DELETE(cube);
	SAFE_DELETE(plane);

	SAFE_DELETE(lightBuffer);
	SAFE_DELETE(shader);
}

void TestAmbient::Update()
{
	sphere->Update();
	sphere2->Update();
	cube->Update();
	plane->Update();
	bunny->Update();
}

void TestAmbient::PreRender()
{

}

void TestAmbient::Render()
{
	lightBuffer->SetPSBuffer(2);

	sphere->Render();
	sphere2->Render();
	cube->Render();
	plane->Render();
	bunny->Render();
}

void TestAmbient::PostRender()
{
	
}

