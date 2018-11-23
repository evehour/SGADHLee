#pragma once
#include "Execute.h"

class TestGeometry : public Execute
{
public:
	TestGeometry();
	~TestGeometry();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	class Terrain* terrain;
	class Sky* sky;

	class Render2D* render2D;
	class RenderTarget* renderTarget;

	Shader* shader;
	ID3D11Buffer* vertexBuffer;
	TextureArray* textureArray;
};