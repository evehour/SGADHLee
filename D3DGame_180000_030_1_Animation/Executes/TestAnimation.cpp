#include "stdafx.h"
#include "TestAnimation.h"
#include "../Objects/GameAnimModel.h"

TestAnimation::TestAnimation(ExecuteValues * values)
	: Execute(values)
{
	kachujin = new GameAnimModel
	(
		Models + L"Kachujin/", L"Kachujin.material",
		Models + L"Kachujin/", L"Kachujin.mesh"
	);
	kachujin->SetShader(new Shader(Shaders + L"035_Animation.hlsl"));
	kachujin->SetNormalMap(Assets + L"Kachujin/Mesh.fbm/Kachujin_normal.png");
	kachujin->SetSpecularMap(Assets + L"Kachujin/Mesh.fbm/Kachujin_specular.png");

	kachujin->AddClip(Models + L"Kachujin/Samba_Dance.anim");
	kachujin->LockRoot(0, false);
	kachujin->Repeat(0, true);
	kachujin->Speed(0, 15.0f);
}

TestAnimation::~TestAnimation()
{
	SAFE_DELETE(kachujin);
}

void TestAnimation::Update()
{
	kachujin->Update();
}

void TestAnimation::PreRender()
{
}

void TestAnimation::Render()
{
	kachujin->Render();
}

void TestAnimation::PostRender()
{
}

