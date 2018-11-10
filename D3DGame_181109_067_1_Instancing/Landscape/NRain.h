#pragma once
class NRain
{
public:
	NRain(ExecuteValues* values);
	~NRain();

	void Update();
	void Render();

private:
	ExecuteValues* values;
	Shader* advancedRainShader;
	Shader* renderParticle;
	
	Texture** textures;
	UINT rainTextureCount = 370;

	float g_DrawFraction = 1.0;
	float g_heightMin = 0.0f;
	float g_heightRange = 40.0f;
	float g_radiusMin = 0.0f;
	float g_radiusRange = 45.0f;
	int g_numRainVertices = 150000;
	float g_WindAmount;
	const float g_constWindAmount1 = 1.0;
	const float g_constWindAmount2 = 0.48;

private:
	bool firstFrame;
	ID3D11Buffer*           g_pParticleStart = NULL;
	ID3D11Buffer*           g_pParticleStreamTo = NULL;
	ID3D11Buffer*           g_pParticleDrawFrom = NULL;


	class GlobalCSShaderBuffer : public ShaderBuffer
	{
	public:
		GlobalCSShaderBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.g_TotalVel = D3DXVECTOR3(0.0f, -0.25f, 0.0f);
			Data.g_FrameRate = 0.0f;
		}

		struct Struct
		{
			D3DXVECTOR3 g_TotalVel;
			float g_FrameRate;
		} Data;
	};
	GlobalCSShaderBuffer* globalCSBuffer;


	class PSBuffer : public ShaderBuffer
	{
	public:
		PSBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.TimeCycle = 0.0f;
		}

		struct Struct
		{
			float TimeCycle;
			float g_rainSplashesXDisplaceShaderVariable;
			float g_rainSplashesYDisplaceShaderVariable;
			float Padding;
		} Data;
	};
	PSBuffer* psBuffer;

	//structures
	struct RainVertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 seed;
		D3DXVECTOR3 speed;
		float random;
		unsigned char  Type;
	};

	WorldBuffer* worldBuffer;

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
	float time;
};