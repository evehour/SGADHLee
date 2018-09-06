#include "stdafx.h"
#include "TestRenderTarget.h"
#include "../Landscape/Sky.h"
#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshCube.h"

#include "../PostEffects/Grayscale.h"

TestRenderTarget::TestRenderTarget(ExecuteValues * values)
	: Execute(values)
{
	sky = new Sky(values);
	shader = new Shader(Shaders + L"026_NormalMap.hlsl");

	renderTarget = new RenderTarget(values);
	//Create Render2D
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		render2D[0] = new Render2D(values);
		render2D[0]->Scale(desc.Width * 0.5f, desc.Height);

		render2D[1] = new Grayscale(values);
		render2D[1]->Position(desc.Width * 0.5f, 0);
		render2D[1]->Scale(desc.Width * 0.5f, desc.Height);
	}

	plane = new MeshPlane();
	plane->Scale(3, 3, 3);
	plane->SetShader(shader);
	plane->SetDiffuse(1, 1, 1, 1);
	plane->SetDiffuseMap(Textures + L"Floor.png");
	plane->SetNormalMap(Textures + L"Floor_normal.png");

	sphere = new MeshSphere();
	sphere->Scale(3, 3, 3);
	sphere->Position(7.0f, 1.5f, 0.0f);
	sphere->SetShader(shader);
	sphere->SetDiffuse(1, 1, 1, 1);
	sphere->SetDiffuseMap(Textures + L"Wall.png");
	sphere->SetNormalMap(Textures + L"Wall_normal.png");

	cube[0] = new MeshCube();
	cube[0]->Scale(2, 2, 2);
	cube[0]->Position(-5.0f, 1.0f, 0.0f);
	cube[0]->SetShader(shader);
	cube[0]->SetDiffuse(1, 1, 1, 1);
	cube[0]->SetDiffuseMap(Textures + L"Bricks.png");
	cube[0]->SetNormalMap(Textures + L"Bricks_normal.png");

	cube[1] = new MeshCube();
	cube[1]->Scale(2, 2, 2);
	cube[1]->Position(0.0f, 1.0f, 0.0f);
	cube[1]->SetShader(shader);
	cube[1]->SetDiffuse(1, 1, 1, 1);
	cube[1]->SetDiffuseMap(Textures + L"Stones.png");
	cube[1]->SetNormalMap(Textures + L"Stones_normal.png");

}

TestRenderTarget::~TestRenderTarget()
{
	SAFE_DELETE(sky);
	SAFE_DELETE(plane);
	SAFE_DELETE(sphere);
	SAFE_DELETE(cube[0]);
	SAFE_DELETE(cube[1]);
	SAFE_DELETE(render2D[0]);
	SAFE_DELETE(render2D[1]);
}

void TestRenderTarget::Update()
{
	sky->Update();
	plane->Update();
	sphere->Update();
	cube[0]->Update();
	cube[1]->Update();
}

void TestRenderTarget::PreRender()
{
	renderTarget->Set();

	sky->Render();
	plane->Render();
	sphere->Render();
	cube[0]->Render();
	cube[1]->Render();

	if (Keyboard::Get()->Down(VK_SPACE))
		renderTarget->SaveTexture(L"Test.png");
}

void TestRenderTarget::Render()
{
}

void TestRenderTarget::PostRender()
{
	for (UINT i = 0; i < 2; i++)
	{
		render2D[i]->Update();
		render2D[i]->SRV(renderTarget->GetSRV());
		render2D[i]->Render();
	}
}

