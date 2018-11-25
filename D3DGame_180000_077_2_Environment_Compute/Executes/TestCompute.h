#pragma once
#include "Execute.h"

class TestCompute : public Execute
{
public:
	TestCompute();
	~TestCompute();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	class Terrain* terrain;

	Shader* shader;
	ID3D11Buffer* vertexBuffer[2];
	TextureArray* textureArray;

private:
	struct VertexWorld
	{
		D3DXMATRIX World;
		UINT Texture;
	};
};