#pragma once

class Base3DParticleInstancer
{
public:
	Base3DParticleInstancer(wstring shaderFName, wstring textureFName);
	~Base3DParticleInstancer();

	void Render();
	void Render(Shader* shader);

	void CalcVertexBuffer();

public:
	map<class Base3DParticleInstance*, D3DXMATRIX> instanceTransformMatrices;
	map<int, class Base3DParticleInstance*> instances;

	D3DXVECTOR2 size;
	D3DXVECTOR3 position, scale;
	D3DXQUATERNION rotation;
	D3DXMATRIX world;

	UINT instanceCount;

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
	DepthStencilState* depthStencilState[2];

	class ParticleInstanceShaderBuffer : public ShaderBuffer
	{
	public:
		ParticleInstanceShaderBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.LightColor = { 1,1,1,1 };
			D3DXMatrixIdentity(&Data.World);
		}

		struct Struct
		{
			D3DXCOLOR LightColor;
			D3DXMATRIX World;
		} Data;
	};
	ParticleInstanceShaderBuffer* particleInstanceShaderBuffer;
};