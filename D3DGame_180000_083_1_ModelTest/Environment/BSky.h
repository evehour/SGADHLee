#pragma once

class BSky
{
public:
	BSky();
	~BSky();

	void Update();
	void Render();

private:
	Material* material;
	MeshSphere* sphere;

	D3DXCOLOR apex;
	D3DXCOLOR center;
	float height;
};