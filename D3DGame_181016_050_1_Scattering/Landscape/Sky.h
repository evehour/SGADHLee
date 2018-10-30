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
	UINT domeCount; // ���� ������ �����
	ID3D11Buffer *vertexBuffer, *indexBuffer;

	// Reset�� 9,10 �� ����̽� �ν�Ʈ�� �߻����� �� �߾���.
	// 11���ʹ� ����.
};