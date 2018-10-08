#pragma once
#include "Execute.h"

class TestMultiTexture : public Execute
{
public:
	TestMultiTexture(ExecuteValues* values);
	~TestMultiTexture();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class MeshQuad* quad;
	Shader* shader;

private:


	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Selected = -1;
		}

		struct Struct
		{
			int Selected;
			float Padding[3];
		} Data;
	};
	Buffer* buffer;
};