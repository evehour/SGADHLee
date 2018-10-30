#pragma once

class Base3DParticleInstancer
{
public:
	Base3DParticleInstancer(wstring shaderFName);
	~Base3DParticleInstancer();

	void Render();
	void Render(Shader* shader);

public:
	map<class Base3DParticleInstance*, D3DXMATRIX> instanceTransformMatrices;
	map<int, class Base3DParticleInstance*> instances;

	wstring shaderFName, textureFName;

	D3DXVECTOR2 size;
	D3DXVECTOR3 position, scale;
	D3DXQUATERNION rotation;
	D3DXMATRIX world;

protected:
	void LoadContent();

protected:
	Shader* shader;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* instanceBuffer;
};