#include "stdafx.h"
#include "TestAnimation.h"

#include "../Animation/BoneAnimaitionTool.h"
#include "../Objects/GameModel.h"

TestAnimaion::TestAnimaion(ExecuteValues* values)
	: Execute(values)
{
	shader = new Shader(Shaders + L"028_Diffuse.hlsl");
	model = new GameModel(
		Models + L"Tank/", L"Tank.material",
		Models + L"Tank/", L"Tank.mesh"
	);
	model->SetShader(shader);

	animationTool = new BoneAnimationTool(values, model);
}

TestAnimaion::~TestAnimaion()
{
	SAFE_DELETE(animationTool);
	SAFE_DELETE(model);
	SAFE_DELETE(shader);
}

void TestAnimaion::Update()
{
	animationTool->Update();
}

void TestAnimaion::Render()
{
	animationTool->Render();
}

void TestAnimaion::PostRender()
{
	animationTool->PostRender();
}
