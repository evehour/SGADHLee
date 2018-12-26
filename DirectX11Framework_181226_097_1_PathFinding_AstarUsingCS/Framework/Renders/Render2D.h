#pragma once

class Render2D
{
public:
	Render2D(UINT pass = 0, float width = 0.0f, float height = 0.0f);
	virtual ~Render2D();

	void Position(const float& x, const float& y);
	void Position(const D3DXVECTOR2& vec);

	void Scale(const float& x, const float& y);
	void Scale(const D3DXVECTOR2& vec);

	D3DXVECTOR2 Scale() { return scale; }

	void Update();
	virtual void Render();
	void UseCenterPosition(bool val) { bCenterPosition = val; }

	void SRV(ID3D11ShaderResourceView* srv);

private:
	void UpdateWorld();

private:
	UINT pass;
	Shader* shader;

	ID3D11Buffer* vertexBuffer;

	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX orthographic;

	bool bCenterPosition;
	float screenWidth;
	float screenHeight;

	D3DXVECTOR2 position;
	D3DXVECTOR2 scale;
};