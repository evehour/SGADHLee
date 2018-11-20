#pragma once
#include "Execute.h"

class TestInstancing : public Execute
{
public:
	TestInstancing(ExecuteValues* values);
	~TestInstancing();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};
	
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* instanceBuffer;

	Shader* shader;
	WorldBuffer* worldBuffer;

	TextureArray* textureArray;
	SamplerState* samplerState;

	RasterizerState* rast;
};