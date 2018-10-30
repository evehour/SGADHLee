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

private:
	ExecuteValues* values;
	bool realTime;

	float theta, phi;
	float prevTheta, prevPhi;

	RenderTarget* mieTarget, *rayleighTarget;
	Shader* scatterShader;
	Shader* targetShader;

	VertexTexture* quadVertices;
	ID3D11Buffer* quadBuffer;

	WorldBuffer* worldBuffer;

	RasterizerState* rasterizeState[2];
	DepthStencilState* depthStencilState[2];

private:
	class ScatterBuffer : public ShaderBuffer
	{
	public:
		ScatterBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{

		}

		struct Struct
		{

		} Data;
	};

	ScatterBuffer* scatterBuffer;

	UINT vertexCount, indexCount;
	UINT domeCount; // 구의 정점이 몇개인지
	ID3D11Buffer *vertexBuffer, *indexBuffer;

	// Reset은 9,10 때 디바이스 로스트가 발생했을 때 했었음.
	// 11부터는 안함.
};