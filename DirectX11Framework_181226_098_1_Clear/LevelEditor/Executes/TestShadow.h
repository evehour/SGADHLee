#pragma once
#include "Systems/IExecute.h"

class TestShadow : public IExecute
{
public:
	// IExecute��(��) ���� ��ӵ�
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
