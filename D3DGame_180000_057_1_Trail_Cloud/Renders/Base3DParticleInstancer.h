#pragma once

struct IntanceDataBuffer
{
	IntanceDataBuffer()
		:matrix(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		)
	{
		instance = 0;
	}

	D3DXMATRIX matrix;

	class Base3DParticleInstance* instance;
	float padding[3];
};

class Base3DParticleInstancer
{
public:
	Base3DParticleInstancer(wstring shaderFName, wstring textureFName);
	~Base3DParticleInstancer();

	void Render();
	void Render(Shader* shader);

	void CalcVertexBuffer();
	void Rotation(D3DXVECTOR3 axis, float angle);

	void SetLightColor(D3DXCOLOR lightColor);
	D3DXCOLOR GetLightColor() { return particleInstanceShaderBuffer->Data.LightColor; }

public:
	vector<struct IntanceDataBuffer> instances;

	D3DXVECTOR2 size;
	D3DXVECTOR3 position, scale;
	D3DXQUATERNION rotation;
	D3DXMATRIX world;

	UINT instanceCount, instanceCountBefore;

protected:
	void LoadContent();

protected:
	wstring shaderFName, textureFName;

	Shader* particleShader;
	Texture* particleTexture;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* instanceBuffer;

	BlendState* blendState[2];
	RasterizerState* rasterizerState[2];
	DepthStencilState* depthStencilState[2];

	WorldBuffer* worldBuffer;

	class ParticleInstanceShaderBuffer : public ShaderBuffer
	{
	public:
		ParticleInstanceShaderBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.LightColor = D3DXCOLOR(1, 1, 1, 1);
		}

		struct Struct
		{
			D3DXCOLOR LightColor;
		} Data;
	};
	ParticleInstanceShaderBuffer* particleInstanceShaderBuffer;
};
