#pragma once
//AreaLight.h

class AreaLight
{
public:
	AreaLight(ExecuteValues * values);
	~AreaLight();

	void Update();
	void Render();
	void PostRender();

public:
	struct Desc
	{
		D3DXVECTOR3 Position;
		float AreaLightWidth;
		D3DXVECTOR3 Direction;
		float AreaLightHeight;
		D3DXVECTOR3 Color;

		float Intensity;
	};

	int pickedIdx;

private:
	D3DXVECTOR3 mScale;
	ExecuteValues * values;

	struct LightPackage
	{
		UINT idx;
		Desc desc;
		class DebugDraw* debugDraw;

		LightPackage()
		{
			idx = 0;
			debugDraw = NULL;
		}
	};
	vector<LightPackage> vLights;

	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 32; i++)
			{
				Data.Lights[i].Position = D3DXVECTOR3(0, 0, 0);
				Data.Lights[i].Color = D3DXVECTOR3(0,0,0);
				Data.Lights[i].AreaLightWidth = 0;
				Data.Lights[i].AreaLightHeight = 0;
				Data.Lights[i].Direction = D3DXVECTOR3(0, 0, 0);
				Data.Lights[i].Intensity = 1;
			}

			Data.Count = 0;
		}

		struct Struct
		{
			Desc Lights[32];
			UINT Count;
			float Padding[3];
		} Data;
	};

	Buffer* buffer;

public:
	void Add(Desc& desc);
};