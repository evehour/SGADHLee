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
	class Grass* grass;
	class Rain* rain;

	Shader* shader;
	ID3D11Buffer* vertexBuffer;
	TextureArray* textureArray;
};