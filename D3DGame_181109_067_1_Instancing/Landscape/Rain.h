#pragma once

class Rain
{
private:
	struct VertexRain;

public:
	Rain(ExecuteValues* values, D3DXVECTOR3& size, UINT count); // � ����wl
	~Rain();

	void Update();
	void Render();

private:
	ExecuteValues* values;

	Shader* shader;

	ID3D11Buffer* vertexBuffer;
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

			Data.Velocity = D3DXVECTOR3(0, 0, 0);
			Data.Color = D3DXCOLOR(1, 1, 1, 1);

			Data.Position = D3DXVECTOR3(0, 0, 0);
			Data.Size = D3DXVECTOR3(0, 0, 0);
		}


		struct Struct
		{
			D3DXVECTOR3 Velocity; // ��� �������� �ѷ�����
			float DrawDistance; // �� �Ÿ� �Ѿ�� �ȱ׸�

			D3DXCOLOR Color; // Ambient

			D3DXVECTOR3 Position; // �� ���� ��ġ
			float Time;

			D3DXVECTOR3 Size;
			float Padding2;
		} Data;
	};
	Buffer* buffer;

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