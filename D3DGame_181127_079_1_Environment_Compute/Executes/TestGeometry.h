#pragma once
#include "Execute.h"

#define __USE_NOMAL_RAIN

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
	class Sky* sky;
	class Terrain* terrain;
	class Grass* grass;
#ifdef __USE_NOMAL_RAIN
	class Rain* rain;
#else
	class NRain* rain;
#endif

	Shader* shader;
	ID3D11Buffer* vertexBuffer;
	TextureArray* textureArray;
};