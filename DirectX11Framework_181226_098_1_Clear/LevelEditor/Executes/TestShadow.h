#pragma once
#include "Systems/IExecute.h"

class TestShadow : public IExecute
{
public:
	// IExecute을(를) 통해 상속됨
	void Initialize() override;
	void Ready() override;
	void Destroy() override;
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void ResizeScreen() override;

private:
	Shader* shader;
	Material* meshMaterial;
	Material* meshShadowMaterial;

	MeshQuad* quad;
	MeshSphere* sphere;

	Render2D* render2D;
};
