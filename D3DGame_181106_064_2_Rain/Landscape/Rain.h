#pragma once

class Rain
{
private:
	struct VertexRain;

public:
	Rain(ExecuteValues* values, D3DXVECTOR3& size,UINT count);
	~Rain();

	void Update();
	void Render();

private:
	ExecuteValues* values;
	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	VertexRain* vertices;

	ID3D11Buffer* indexBuffer;
	UINT* indices;

	UINT vertexCount;
	UINT indexCount;
	UINT particleCount;

	Texture* rainMap;
	BlendState* blendState[2];

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.DrawDistance = 1000.0f;
			Data.Velocity = D3DXVECTOR3(0, 0, 0); // 어느방향으로 뿌려질지
			Data.Color = D3DXCOLOR(1, 1, 1, 1);

			Data.Position = D3DXVECTOR3(0, 0, 0);
			Data.Size = D3DXVECTOR3(0, 0, 0);
		}

		struct Struct
		{
			D3DXVECTOR3 Velocity;
			float DrawDistance; // 이 거리 밖은 안그림.

			D3DXCOLOR Color; // Ambient

			D3DXVECTOR3 Position; // 피가 나올 위치.
			float Time;

			D3DXVECTOR3 Size; // 큐브의 사이즈
			float Padding2;

		} Data;
	};
	Buffer* buffer;

private:
	struct VertexRain
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR2 Uv;
		D3DXVECTOR2 Scale;

		VertexRain()
		{
			Position = D3DXVECTOR3(0, 0, 0);
			Uv = D3DXVECTOR2(0, 0);
			Scale = D3DXVECTOR2(0, 0);
		}

		VertexRain(D3DXVECTOR3& position, D3DXVECTOR2& uv, D3DXVECTOR2& scale)
		{
			Position = position;
			Uv = uv;
			Scale = scale;
		}
	};
};