#pragma once

#define RAIN_USING_DRAWAUTO

class NRain
{
public:
	NRain(ExecuteValues* values);
	~NRain();

	void Update();
	void Render();

private:
	HRESULT GenFogTable(string fileName, int xRes, int yRes, ID3D11Texture2D** fogTexture2D, ID3D11ShaderResourceView** fogShaderResourceView);

private:
	ExecuteValues* values;
	ID3D11Buffer* initBuffer;
	ID3D11Buffer* drawVBuffer;
	ID3D11Buffer* SOVBuffer;

	Shader *shader1, *shader2;

	RasterizerState* rasterizerState[2];
	DepthStencilState* depthStencilState[2];
	BlendState* blendState[2];

	TextureArray* textureArray;

	UINT rainTextureCount = 370;

	//Fog
	ID3D11Texture2D* fogTexture;
	ID3D11ShaderResourceView* fogSRV;

private:

	float g_DrawFraction = 1.0f;
	float g_heightMin = 0.0f;
	float g_heightRange = 40.0f;
	float g_radiusMin = 0.0f;
	float g_radiusRange = 45.0f;
	int g_numRainVertices = 150000;
	float g_WindAmount = 0.48f;
	const float g_constWindAmount1 = 1.0f;
	const float g_constWindAmount2 = 0.48f;

	//structures
	struct RainVertex
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Seed;
		D3DXVECTOR3 Speed;
		float Random;
		UINT  Type;
	};

	WorldBuffer* worldBuffer;

	class PSBuffer : public ShaderBuffer
	{
	public:
		PSBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.TimeCycle = 0.0f;
			Data.g_rainSplashesXDisplaceShaderVariable = 0.0f;
			Data.g_rainSplashesYDisplaceShaderVariable = 0.0f;

			Data.Beta = D3DXVECTOR3(0.4f, 0.4f, 0.4f);
			Data.XOffset = 0.0f;
			Data.XScale = (float)(1.0 / D3DX_PI / 2.0);
			Data.YOffset = 0.0f;
			Data.YScale = 0.5f;
		}

		struct Struct
		{
			float TimeCycle;
			float g_rainSplashesXDisplaceShaderVariable;
			float g_rainSplashesYDisplaceShaderVariable;
			float Padding;

			D3DXVECTOR3 Beta;
			float Padding2;

			float XOffset;
			float XScale;
			float YOffset;
			float YScale;
		} Data;
	};
	PSBuffer* psBuffer;

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
	float g_time;

private:
	bool bFirstRender;
};