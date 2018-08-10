#pragma once
#include "Execute.h"

class Cube : public Execute
{

private:
	class ColorBuffer;

public:
	Cube(ExecuteValues* values);
	~Cube();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	void SetPosition(D3DXVECTOR3 pos);
	void GetPosition(D3DXVECTOR3 *pos);
	void SetScale(D3DXVECTOR3 scale);
	void GetScale(D3DXVECTOR3 *scale);
	void GetRotate(D3DXVECTOR3 *rotate);

	void GetFoward(D3DXVECTOR3 *foward);
	void SetFoward(D3DXVECTOR3 foward);
	void GetRight(D3DXVECTOR3 *right);
	void SetRight(D3DXVECTOR3 right);

private:
	D3DXVECTOR3 m_vec3Position;
	D3DXVECTOR3 m_vec3Rotate;
	D3DXVECTOR3 m_vec3Scale;

	D3DXVECTOR3 m_foward, m_right;

private:
	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT vertexCount, indexCount;

	VertexTextureNormal* vertices;
	UINT* indices;

	ColorBuffer* colorBuffer;

	ID3D11ShaderResourceView* srv;

private:
	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 1, 1, 1);
		}
		~ColorBuffer()
		{

		}

		struct Struct
		{
			D3DXCOLOR Color;
		}Data;
	};
};