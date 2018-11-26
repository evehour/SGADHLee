#pragma once

class Render2D
{
public:
	Render2D(wstring shaderFile = L"", string vsName = "VS", string psName = "PS");
	virtual ~Render2D();

	void Position(const float& x, const float& y);
	void Position(const D3DXVECTOR2& vec);

	void Scale(const float& x, const float& y);
	void Scale(const D3DXVECTOR2& vec);

	D3DXVECTOR2 Scale() { return scale; }

	void Update();
	virtual void Render();

	void SRV(ID3D11ShaderResourceView* srv) { this->srv = srv; }

private:
	Shader* shader;
	ID3D11ShaderResourceView* srv;

	ID3D11Buffer* vertexBuffer;

	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX orthographic;

	D3DXVECTOR2 position;
	D3DXVECTOR2 scale;
};