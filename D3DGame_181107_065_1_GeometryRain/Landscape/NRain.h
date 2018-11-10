#pragma once
class NRain
{
public:
	NRain(ExecuteValues* values);
	~NRain();


private:
	ExecuteValues* values;
	Shader* shader;
	
	Texture** textures;
	UINT rainTextureCount = 370;

	float g_heightMin = 0.0f;
	float g_heightRange = 40.0f;
	float g_radiusMin = 0.0f;
	float g_radiusRange = 45.0f;
	int g_numRainVertices = 150000;
private:
	ID3D11Buffer*           g_pParticleStart = NULL;
	ID3D11Buffer*           g_pParticleStreamTo = NULL;
	ID3D11Buffer*           g_pParticleDrawFrom = NULL;

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
			D3DXVECTOR3 Velocity; // 어느 방향으로 뿌려질지
			float DrawDistance; // 이 거리 넘어가면 안그림

			D3DXCOLOR Color; // Ambient

			D3DXVECTOR3 Position; // 비가 나올 위치
			float Time;

			D3DXVECTOR3 Size;
			float Padding2;
		} Data;
	};
	Buffer* buffer;

	//structures
	struct RainVertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 seed;
		D3DXVECTOR3 speed;
		float random;
		unsigned char  Type;
	};

	struct SimpleVertex
	{
		D3DXVECTOR3 Pos; // Position
		D3DXVECTOR2 Tex; // Texture Coordinate
	};

	struct WindValue
	{
		D3DXVECTOR3 windAmount;
		int time;
		WindValue(D3DXVECTOR3 wA, int t)
		{
			windAmount = wA; time = t;
		}
	};
	vector<WindValue> WindAnimation;
	int totalAnimationValues = 0;
};