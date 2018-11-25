#pragma once

class Rain
{
private:
	struct VertexRain;

public:
	Rain(D3DXVECTOR3& size, UINT count = 2000); // 몇개 만들wl
	~Rain();

	void Update();
	void Render();

private:
	Shader* shader;

	ID3D11Buffer* drawBuffer[2];
	int particleCount;

	Texture* rainMap;

private:
	struct ShaderBuffer
	{
		D3DXVECTOR3 Velocity; // 어느 방향으로 뿌려질지
		float DrawDistance; // 이 거리 넘어가면 안그림

		D3DXCOLOR Color; // Ambient

		D3DXVECTOR3 OriginPosition; // 비가 나올 위치
		float Time;

		D3DXVECTOR3 AreaSize;

		ShaderBuffer()
		{
			DrawDistance = 1000.0f;

			Velocity = D3DXVECTOR3(0, 0, 0);
			Color = D3DXCOLOR(1, 1, 1, 1);

			OriginPosition = D3DXVECTOR3(0, 0, 0);
			AreaSize = D3DXVECTOR3(0, 0, 0);
			Time = 0.0f;
		}
	};
	ShaderBuffer rainBuffer;

	struct VertexRain
	{
		VertexRain()
		{
			Position = D3DXVECTOR3(0, 0, 0);
			Scale = D3DXVECTOR2(0, 0);
		}

		VertexRain(D3DXVECTOR3& position, D3DXVECTOR2 scale)
		{
			Position = position;
			Scale = scale;
		}

		D3DXVECTOR3 Position;
		D3DXVECTOR2 Scale;
	};
};