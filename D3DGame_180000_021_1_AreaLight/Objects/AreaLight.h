#pragma once
//AreaLight.h

class AreaLight
{
public:
	AreaLight();
	~AreaLight();

	void Update();

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

private:
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
	void Add(Desc& desc)
	{
		int count = buffer->Data.Count;
		buffer->Data.Lights[count] = desc;

		buffer->Data.Count++;
	}
};