#pragma once

class Render2D
{
public:
	Render2D(struct ExecuteValues* values, wstring shaderFile = L"", string vsName = "VS", string psName = "PS");
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
	struct ExecuteValues* values;

	Shader* shader;
	ID3D11ShaderResourceView* srv;

	class WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	class DepthStencilState* depthState[2];
	class SamplerState* samplerState;

	D3DXMATRIX view;
	D3DXMATRIX orthographic;

	D3DXVECTOR2 position;
	D3DXVECTOR2 scale;
};