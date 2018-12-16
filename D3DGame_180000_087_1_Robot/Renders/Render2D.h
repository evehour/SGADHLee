#pragma once
#include "./Objects/GameRender.h"

class Render2D : public GameRender
{
public:
	Render2D(wstring shaderFile = L"");
	virtual ~Render2D();

	void Update();
	virtual void Render();

	void SRV(ID3D11ShaderResourceView* srv);

private:
	void UpdateWorld() override;

private:
	Shader* shader;

	ID3D11Buffer* vertexBuffer;

	D3DXMATRIX view;
	D3DXMATRIX orthographic;
};