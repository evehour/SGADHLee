#include "stdafx.h"
#include "VolumedCloud.h"

#include "../Renders/Base3DParticleInstance.h"
#include "../Renders/Base3DParticleInstancer.h"

VolumedCloud::VolumedCloud(wstring shaderFile, wstring Texture, SkyType skyType, ExecuteValues* values)
	: values(values)
	, timeStorage(0.0f)
{
	clouds = new Base3DParticleInstancer(shaderFile, Texture);

	float x, y, z;
	float d = 1.0f;
	float boxSize;
	D3DXVECTOR3 flatBase = { 10,1,5 };
	D3DXVECTOR3 cloudDim1;
	D3DXVECTOR3 episode1PlayArea;

	speedTran = 1.0f;
	speedRot = 0.01f;

	switch (skyType)
	{
	case SkyType::CloudSplatter:
		for (int c = 0; c < 100; c++)
		{
			boxSize = 250.0f;
			d = 1;
			Math::Lerp(x, -boxSize, boxSize, Math::Random(0.0f, 1.0f));
			Math::Lerp(y, -boxSize / 2.0f, boxSize, Math::Random(0.0f, 1.0f));
			Math::Lerp(z, -boxSize, boxSize, Math::Random(0.0f, 1.0f));

			if (y < 200.0f)
				d = 0.75f;
			if (y < 0.0f)
				d = 0.5f;

			tmpRange = { 0,1,2,3,4,5,6,7,8,9 };
			AddCloud(25, D3DXVECTOR3(x, y, z), 40, flatBase, flatBase * 5.0f, d, tmpRange);
		}
		break;

	case SkyType::CloudField:
		cloudDim1 = D3DXVECTOR3(500, 20, 500);

		tmpRange = { 0, 1, 2, 3, 4 };
		AddCloud(2000, D3DXVECTOR3(0, 0, 0), 60, cloudDim1, cloudDim1, .25f, tmpRange);

		tmpRange = { 3, 4, 5, 6, 7, 8 };
		AddCloud(2000, D3DXVECTOR3(0, 30, 0), 60, cloudDim1, cloudDim1, .5f, tmpRange);

		tmpRange = { 7, 8, 9, 10, 11 };
		AddCloud(2000, D3DXVECTOR3(0, 60, 0), 60, cloudDim1, cloudDim1, .75f, tmpRange);

		tmpRange = { 0, 1, 2, 3, 4, 12, 13, 14, 15 };
		AddCloud(2000, D3DXVECTOR3(0, 90, 0), 60, cloudDim1, cloudDim1, 1.0f, tmpRange);
		break;

	case SkyType::CloudySky:
		episode1PlayArea = D3DXVECTOR3(1000, 1000, 1000);
		tmpRange = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

		// Outer large clouds                    
		AddCloud(50, D3DXVECTOR3(0, 0, 0), 2500, D3DXVECTOR3(4000, 4000, 4000), D3DXVECTOR3(2000, 2000, 2000), 0.75f, tmpRange);

		// clouds inplay
		flatBase = D3DXVECTOR3(50, 5, 25);

		for (int c = 0; c < 50; c++)
		{
			d = 1;

			Math::Lerp(x, -episode1PlayArea.x, episode1PlayArea.x, Math::Random(0.0f, 1.0f));
			Math::Lerp(y, -episode1PlayArea.y, episode1PlayArea.y, Math::Random(0.0f, 1.0f));
			Math::Lerp(z, -episode1PlayArea.z, episode1PlayArea.z, Math::Random(0.0f, 1.0f));

			if (y < 200)
				d = .8f;
			if (y < 0 && y > -500)
				d = .75f;
			if (y < -500)
				d = .5f;

			tmpRange = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			AddCloud(25, D3DXVECTOR3(x, y, z), 300, flatBase, flatBase * 5, d, tmpRange);

		}
		break;
	case SkyType::SpotClouds:
		flatBase = { 20,2,20 };
		boxSize = 800;

		for (int c = 0; c < 300; c++)
		{
			Math::Lerp(x, -boxSize, boxSize, Math::Random(0.0f, 1.0f));
			Math::Lerp(y, 0, boxSize, Math::Random(0.0f, 1.0f));
			Math::Lerp(z, -boxSize, boxSize, Math::Random(0.0f, 1.0f));

			vector<int> tmpRange = { 0,1,2,3,4,5,6,7,8,9 };
			AddCloud(25, D3DXVECTOR3(x, y, z), 64, flatBase, flatBase * 5.0f, 0.75f, tmpRange);
		}
		break;
	}

	SortClouds();
}

VolumedCloud::~VolumedCloud()
{
	SAFE_DELETE(clouds);
}

void VolumedCloud::Update()
{
	clouds->Rotation(D3DXVECTOR3(0, 1, 0), 0.0005f * Time::Delta());
#if true
	// 이렇게하면 구름 죠짐...
	timeStorage += Time::Delta();

	if (timeStorage >= 0.01f)
	{
		timeStorage -= 0.01f;
		SortClouds();
	}
#else
	SortClouds();
#endif
}

void VolumedCloud::Render()
{
	clouds->Render();
}

void VolumedCloud::SetCloudColor(D3DXCOLOR color)
{
	clouds->SetLightColor(color);
}

D3DXCOLOR VolumedCloud::GetCloudColor()
{
	return clouds->GetLightColor();
}

void VolumedCloud::AddCloud(int whispCount, D3DXVECTOR3 position, float size, D3DXVECTOR3 min, D3DXVECTOR3 max, float colorMod, vector<int> whispRange)
{
	UINT si = 0;
	float scaleMod;
	Math::GetDistance(scaleMod, -min, max);
	scaleMod /= 4.5f;

	float x, y, z;
	for (int w = 0; w < whispCount; w++)
	{
		Math::Lerp(x, -min.x, max.x, Math::Random(0.0f, 1.0f));
		Math::Lerp(y, -min.y, max.y, Math::Random(0.0f, 1.0f));
		Math::Lerp(z, -min.z, max.z, Math::Random(0.0f, 1.0f));

		if (si >= whispRange.size())
			si = 0;

		Base3DParticleInstance* inst = new Base3DParticleInstance
		(
			position + D3DXVECTOR3(x, y, z),
			D3DXVECTOR3(1, 1, 1) * size,
			D3DXVECTOR3(whispRange[si++] / 100.0f, 1.0f, (((float)Math::Random((int)7, (int)10) / 10.0f) * colorMod)),
			clouds
		);
		whisps.push_back(inst);
	}
}

void VolumedCloud::AddCloud(int whispCount, D3DXVECTOR3 position, float size, float radius, float colorMod, vector<int> whispRange)
{
	UINT si = 0;
	float scaleMod;
	Math::GetDistance(scaleMod, position, (position * radius));
	scaleMod /= 4.5f;

	float x, y, z;
	for (int w = 0; w < whispCount; w++)
	{
		Math::Lerp(x, -radius, radius, Math::Random(0.0f, 1.0f));
		Math::Lerp(y, -radius, radius, Math::Random(0.0f, 1.0f));
		Math::Lerp(z, -radius, radius, Math::Random(0.0f, 1.0f));

		if (si >= whispRange.size())
			si = 0;

		Base3DParticleInstance* inst = new Base3DParticleInstance
		(
			position + D3DXVECTOR3(x, y, z),
			D3DXVECTOR3(1, 1, 1) * size,
			D3DXVECTOR3(whispRange[si++] / 100.0f, 1.0f, (((float)Math::Random((int)7, (int)10) / 10.0f) * colorMod)),
			clouds
		);
		whisps.push_back(inst);
	}
}

bool CompareTo(IntanceDataBuffer a, IntanceDataBuffer b)
{

	return (a.instance->distance > a.instance->distance);
}

float CalcDistance(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	float dist;
	dist = (v1.x - v2.x) + (v1.y - v2.y) + (v1.z - v2.z);

	return dist * dist;
}

void VolumedCloud::SortClouds()
{
	D3DXVECTOR3 camPos;
	D3DXMATRIX cloudWorld = clouds->world;

	values->MainCamera->Position(&camPos);

	for (UINT p = 0; p < whisps.size(); p++)
	{
		D3DXMATRIX whispMat = clouds->instances[p].instance->world;
		D3DXVECTOR3 whispPos = { whispMat._41, whispMat._42, whispMat._43 };

		clouds->instances[p].matrix = whispMat;
		clouds->instances[p].instance->distance = CalcDistance(whispPos, camPos);
	}

	std::sort(clouds->instances.begin(), clouds->instances.end(), CompareTo);

	clouds->CalcVertexBuffer();
}
