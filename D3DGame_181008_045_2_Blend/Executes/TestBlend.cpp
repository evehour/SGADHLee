#include "stdafx.h"
#include "TestBlend.h"

#include "../Objects/MeshQuad.h"

TestBlend::TestBlend(ExecuteValues * values)
	: Execute(values)
{
	shader = new Shader(Shaders + L"041_BaseTexture.hlsl");

	quad = new MeshQuad();
	quad->SetShader(shader);
	quad->Scale(10, 10, 10);
	quad->SetDiffuseMap(Textures + L"Floor.png");

	quad2 = new MeshQuad();
	quad2->SetShader(shader);
	quad2->Scale(10, 10, 10);
	quad2->Position(0, 0, 0.001);
	quad2->SetDiffuseMap(Textures + L"BlendTest.png");

	buffer = new Buffer();

	blendState[0] = new BlendState();

	blendState[1] = new BlendState();
	blendState[1]->BlendEnable(true);

	blendState[2] = new BlendState();
	blendState[2]->AlphaToCoverageEnable(true);
	blendState[2]->BlendEnable(true);
}

TestBlend::~TestBlend()
{
	SAFE_DELETE(buffer);

	SAFE_DELETE(shader);

	SAFE_DELETE(quad);
	SAFE_DELETE(quad2);

	SAFE_DELETE(blendState[0]);
	SAFE_DELETE(blendState[1]);
}

void TestBlend::Update()
{
	quad->Update();
	quad2->Update();
}

void TestBlend::PreRender()
{

}

void TestBlend::Render()
{
	ImGui::SliderInt("AlphaBlend", &buffer->Data.Selected, 0, 2);

	quad->Render();

	blendState[buffer->Data.Selected]->OMSetBlendState();
	quad2->Render();
	blendState[0]->OMSetBlendState();
}

void TestBlend::PostRender()
{
	
}

void TestBlend::ResizeScreen()
{

}

