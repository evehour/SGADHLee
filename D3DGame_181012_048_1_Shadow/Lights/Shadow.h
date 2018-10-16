#pragma once
class Shadow
{
public:
	// 투영영역
	//const float Width = 2048;
	//const float Height = 2048;

public:
	Shadow(ExecuteValues* values);
	~Shadow();

	void Add(class GameModel* obj);

	void Update();
	void PreRender();
	void Render();

private:
	ExecuteValues* values;

	Shader* shaderDepth;
	RenderTarget* depthShadow;
	vector<class GameModel*> objs;

	Shader * shaderRender;

	class Render2D* depthRender;
	class Perspective* perspective;
	ViewProjectionBuffer* vpBuffer;

	SamplerState* samplerState[2];

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Bias = 0.0005f;
			Data.Selected = 0;
		}

		struct Struct
		{
			float Bias;
			int Selected;

			float Padding[2];
		} Data;
	};

	Buffer* buffer;
};