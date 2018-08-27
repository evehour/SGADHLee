#include "stdafx.h"
#include "MeshSphere.h"

MeshSphere::MeshSphere()
	: GameModel(Materials + L"Meshes/", L"Sphere.material", Models + L"Meshes/", L"Sphere.mesh")
{
	shader = new Shader(Shaders + L"017_Sphere.hlsl");
	
	for (Material* material : model->Materials())
	{
		material->SetDiffuse(D3DXCOLOR(1, 0, 0, 1));
		material->SetShader(shader);
	}
}

MeshSphere::~MeshSphere()
{
	SAFE_DELETE(shader);
}

void MeshSphere::Update()
{
	__super::Update();
}

void MeshSphere::Render()
{
	__super::Render();
}
