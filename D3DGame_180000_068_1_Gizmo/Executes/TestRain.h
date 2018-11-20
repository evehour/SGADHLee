#pragma once
#include "Execute.h"

class TestRain : public Execute
{
public:
	TestRain(ExecuteValues* values);
	~TestRain();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	class Material* material;

	class MeshGrid* grid;

	Shader* surfaceShader;
	Texture* splash;
	Texture* splashNormal;

	class NRain* rain;

private:
	//Light
	enum class ELightType
	{
		None, Point, Spot, Capsule
	};

	struct LightDesc
	{
		ELightType Type;
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Direction;
		float Range;
		float Length;
		float Outer;
		float Inner;
		D3DXVECTOR3 Color;
	};

	struct LightData
	{
		D3DXVECTOR4 PositionX;
		D3DXVECTOR4 PositionY;
		D3DXVECTOR4 PositionZ;

		D3DXVECTOR4 DirectionX;
		D3DXVECTOR4 DirectionY;
		D3DXVECTOR4 DirectionZ;

		D3DXVECTOR4 Range;

		D3DXVECTOR4 SpotOuter;
		D3DXVECTOR4 SpotInner;

		D3DXVECTOR4 CapsuleLength;

		D3DXVECTOR4 ColorR;
		D3DXVECTOR4 ColorG;
		D3DXVECTOR4 ColorB;

		LightData()
		{
			PositionX = D3DXVECTOR4(0, 0, 0, 0);
			PositionY = D3DXVECTOR4(0, 0, 0, 0);
			PositionZ = D3DXVECTOR4(0, 0, 0, 0);

			DirectionX = D3DXVECTOR4(0, 0, 0, 0);
			DirectionY = D3DXVECTOR4(0, 0, 0, 0);
			DirectionZ = D3DXVECTOR4(0, 0, 0, 0);

			Range = D3DXVECTOR4(0, 0, 0, 0);

			SpotOuter = D3DXVECTOR4(-2.0f, -2.0f, -2.0f, -2.0f);
			SpotInner = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

			CapsuleLength = D3DXVECTOR4(0, 0, 0, 0);

			ColorR = D3DXVECTOR4(0, 0, 0, 0);
			ColorG = D3DXVECTOR4(0, 0, 0, 0);
			ColorB = D3DXVECTOR4(0, 0, 0, 0);
		}

		void Setting(UINT index, LightDesc& desc)
		{
			PositionX[index] = desc.Position.x;
			PositionY[index] = desc.Position.y;
			PositionZ[index] = desc.Position.z;

			if (desc.Type == ELightType::Spot)
			{
				DirectionX[index] = -desc.Direction.x;
				DirectionY[index] = -desc.Direction.y;
				DirectionZ[index] = -desc.Direction.z;
			}
			else if (desc.Type == ELightType::Capsule)
			{
				DirectionX[index] = desc.Direction.x;
				DirectionY[index] = desc.Direction.y;
				DirectionZ[index] = desc.Direction.z;
			}

			Range[index] = 1.0f / desc.Range;

			SpotOuter[index] = desc.Type == ELightType::Spot ? cosf(desc.Outer) : -2.0f; // Spot이 아니면 안쓰겠다는 뜻임.
			SpotInner[index] = desc.Type == ELightType::Spot ? 1.0f / cosf(desc.Inner) : 1.0f; // Spot이 아니면 안쓰겠다는 뜻임.

			if (desc.Type == ELightType::Capsule)
				CapsuleLength[index] = desc.Length;

			ColorR[index] = desc.Color.x * desc.Color.x;
			ColorG[index] = desc.Color.y * desc.Color.y;
			ColorB[index] = desc.Color.z * desc.Color.z;
		}
	};

	class LightBuffer : public ShaderBuffer
	{
	public:
		LightBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			//ZeroMemory(Data.Lights, sizeof(LightData) * 6);
			Data.Count = 0;
		}

		void UseCount(UINT count) { Data.Count = count; }

		void AddLight(const LightData & data)
		{
			Data.Lights[Data.Count] = data;
			Data.Count++;
		}

	private:
		struct Struct
		{
			LightData Lights[6];

			int Count;
			float Padding[3];
		} Data;
	};
	LightBuffer* lightBuffer;
};