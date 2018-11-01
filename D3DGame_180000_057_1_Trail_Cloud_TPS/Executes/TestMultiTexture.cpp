#include "stdafx.h"
#include "TestMultiTexture.h"

#include "../Objects/MeshQuad.h"

TestMultiTexture::TestMultiTexture(ExecuteValues * values)
	: Execute(values)
{
	quad = new MeshQuad();
	shader = new Shader(Shaders + L"041_MultiTexture.hlsl");
	quad->SetShader(shader);
	quad->Scale(10, 10, 10);
	quad->RotationDegree(45, 0, 0);
	//quad->Rotation(Math::ToRadian(90), 0, 0);

	quad->SetDiffuseMap(Textures + L"Floor.png");
	quad->SetNormalMap(Textures + L"Dirt.png");

	buffer = new Buffer();
}

TestMultiTexture::~TestMultiTexture()
{
	SAFE_DELETE(buffer);

	SAFE_DELETE(shader);

	SAFE_DELETE(quad);
}

void TestMultiTexture::Update()
{
	quad->Update();
}

void TestMultiTexture::PreRender()
{

}

void TestMultiTexture::Render()
{
	ImGui::SliderInt("MultiTexture", &buffer->Data.Selected, -1, 11);
	buffer->SetPSBuffer(10);
	quad->Render();
}

void TestMultiTexture::PostRender()
{
	
}

void TestMultiTexture::ResizeScreen()
{

}

