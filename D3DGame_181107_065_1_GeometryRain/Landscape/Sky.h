#pragma once

class Sky
{
public:
	Sky(ExecuteValues* values);
	~Sky();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	void GenerateSphere();
	void GenerateQuad();
	float GetStarIntensity();
	D3DXVECTOR3 GetDirection();

private:
	ExecuteValues* values;
	bool realTime;

	float theta, phi;
	float prevTheta, prevPhi;

	RenderTarget* mieTarget, *rayleighTarget;
	Shader* scatterShader;
	Shader* targetShader;
	Shader* moonShader;
	Shader* cloudShader;

	UINT radius, slices, stacks;

	VertexTexture* quadVertices;
	ID3D11Buffer* quadBuffer;

	Render2D* rayleigh2D;
	Render2D* mie2D;
	Render2D* noise2D;

	WorldBuffer* worldBuffer;
	WorldBuffer* moonWorldBuffer;

	DepthStencilState* depthStencilState[2];
	RasterizerState* rasterizerState[2];
	BlendState* alphaBlend[2];

	Texture* starField;
	Texture* moon;
	Texture* moonGlow;
	Texture* cloude;

	SamplerState* rayleighSampler;
	SamplerState* mieSampler;
	SamplerState* starfieldSampler;
	SamplerState* moonSampler;

private:
	float timeFactor;

private:
	class ScatterBuffer : public ShaderBuffer
	{
	public:
		ScatterBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.StarIntensity = 0.0f;
			Data.MoonAlpha = 0.0f;
		}

		struct Struct
		{
			float StarIntensity;
			float MoonAlpha;
			float padding[2];
		} Data;
	};
	ScatterBuffer* scatterBuffer;

	class TargetBuffer : public ShaderBuffer
	{
	public:
		TargetBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.SampleCount = 20;

			Data.WaveLength = D3DXVECTOR3(0.65f, 0.57f, 0.475f);
			Data.InvWaveLength.x = 1.0f / powf(Data.WaveLength.x, 4.0f);
			Data.InvWaveLength.y = 1.0f / powf(Data.WaveLength.y, 4.0f);
			Data.InvWaveLength.z = 1.0f / powf(Data.WaveLength.z, 4.0f);

			Data.WaveLengthMie.x = powf(Data.WaveLength.x, -0.84f);
			Data.WaveLengthMie.y = powf(Data.WaveLength.y, -0.84f);
			Data.WaveLengthMie.z = powf(Data.WaveLength.z, -0.84f);
		}

		struct Struct
		{
			D3DXVECTOR3 WaveLength;
			int SampleCount;
			D3DXVECTOR3 InvWaveLength;
			float Padding2;
			D3DXVECTOR3 WaveLengthMie;
			float Padding3;
		} Data;
	};
	TargetBuffer* targetBuffer;

	UINT vertexCount, indexCount;
	ID3D11Buffer *vertexBuffer, *indexBuffer;

	// Reset은 9,10 때 디바이스 로스트가 발생했을 때 했었음.
	// 11부터는 안함.
};