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
	vector<class GameModel*> renderObjs;

	Shader * shaderRender;

	class Render2D* depthRender;
	class Perspective* perspective;
	ViewProjectionBuffer* vpBuffer;

	RasterizerState* rasterizerState[2];
	SamplerState* samplerState[2];

	D3D11_SAMPLER_DESC desc;

	ID3D11SamplerState* state;

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
			D3DXVECTOR2 Size;
			float Bias;
			int Selected;
		} Data;
	};

	Buffer* buffer;
};