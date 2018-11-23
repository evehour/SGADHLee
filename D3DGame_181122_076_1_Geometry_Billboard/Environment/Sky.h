#pragma once

class Sky
{
public:
	Sky();
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
	bool realTime;

	float theta, phi;
	float prevTheta, prevPhi;

	Shader* shader;
	RenderTarget* mieTarget, *rayleighTarget;

	UINT radius, slices, stacks;

	VertexTexture* quadVertices;
	ID3D11Buffer* quadBuffer;

	Render2D* rayleigh2D;
	Render2D* mie2D;
	Render2D* noise2D;

	D3DXMATRIX world;

	Texture* starField;
	Texture* moon;
	Texture* moonGlow;
	Texture* cloud;

private:
	float timeFactor;
	D3DXVECTOR3 lightDirection;

private:
	float StarIntensity;
	float MoonAlpha;

	D3DXVECTOR3 WaveLength;
	int SampleCount;
	D3DXVECTOR3 InvWaveLength;
	D3DXVECTOR3 WaveLengthMie;

	UINT vertexCount, indexCount;
	ID3D11Buffer *vertexBuffer, *indexBuffer;

	// Reset은 9,10 때 디바이스 로스트가 발생했을 때 했었음.
	// 11부터는 안함.
};