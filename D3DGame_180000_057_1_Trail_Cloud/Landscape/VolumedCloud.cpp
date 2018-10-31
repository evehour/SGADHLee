#include "stdafx.h"
#include "VolumedCloud.h"

#include "../Renders/Base3DParticleInstance.h"
#include "../Renders/Base3DParticleInstancer.h"

VolumedCloud::VolumedCloud(wstring shaderFile, wstring Texture, SkyType skyType, ExecuteValues* values)
	: values(values)
{
	clouds = new Base3DParticleInstancer(shaderFile, Texture);

	float x, y, z;
	float d = 1.0f;
	D3DXVECTOR3 flatBase = { 10,1,5 };
	float boxSize;

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

			vector<int> tmpRange = { 0,1,2,3,4,5,6,7,8,9 };
			AddCloud(25, D3DXVECTOR3(x, y, z), 40, flatBase, flatBase * 5.0f, d, tmpRange);
		}
		break;
	case SkyType::CloudField:
		break;
	case SkyType::CloudySky:
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

}

VolumedCloud::~VolumedCloud()
{
}

void VolumedCloud::Update()
{
	SortClouds();
}

void VolumedCloud::Render()
{
	clouds->Render();
}

void VolumedCloud::AddCloud(int whispCount, D3DXVECTOR3 position, float size, D3DXVECTOR3 min, D3DXVECTOR3 max, float colorMod, vector<int> whispRange)
{
	int si = 0;
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

		Base3DParticleInstance* inst = new Base3DParticleInstance(position + D3DXVECTOR3(x, y, z), D3DXVECTOR3(1, 1, 1) * size, D3DXVECTOR3(whispRange[si++] / 100.0f, 1.0f, (float)Math::Random((int)7, (int)10) * colorMod), clouds);
		whisps.push_back(inst);
	}
}

void VolumedCloud::AddCloud(int whispCount, D3DXVECTOR3 position, float size, float radius, float colorMod, vector<int> whispRange)
{
	int si = 0;
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

		Base3DParticleInstance* inst = new Base3DParticleInstance(position + D3DXVECTOR3(x, y, z), D3DXVECTOR3(1, 1, 1) * size, D3DXVECTOR3(whispRange[si++] / 100.0f, 1.0f, ((Math::Random((int)7, (int)10) / 10.0f) * colorMod)), clouds);
		whisps.push_back(inst);
	}
}

struct DistData
{
	float dist;
	Base3DParticleInstance* idx;

public:
	DistData() {};

	DistData(Base3DParticleInstance* idx, float dist)
		: idx(idx), dist(dist)
	{
	}

	//bool Compare(DistData& a, DistData& b)
	//{
	//	return a.dist > b.dist;
	//}
	bool operator<(const DistData& other) const { return dist > other.dist; }

	float CalcDistance(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
	{
		float dist;
		//Math::GetDistance(dist, v1, v2);
		dist = (v1.x - v2.x) + (v1.y - v2.y) + (v1.z - v2.z);

		return dist * dist;
	}
};

void VolumedCloud::SortClouds()
{
	vector<DistData> bbDists;
	D3DXVECTOR3 camPos;

	values->MainCamera->Position(&camPos);

	for (int p = 0; p < whisps.size(); p++)
	{
		DistData d;
		D3DXMATRIX whispMat = clouds->instanceTransformMatrices[whisps[p]];
		D3DXVECTOR3 whispPos = { whispMat._41, whispMat._42, whispMat._43 };
		float dist = d.CalcDistance(whispPos, camPos);

		bbDists.push_back(DistData(whisps[p], dist));
	}

	std::sort(bbDists.begin(), bbDists.end());

	clouds->instanceTransformMatrices.clear();
	//clouds->instances.clear();

	for (int p = 0; p < bbDists.size(); p++)
	{
		clouds->instanceTransformMatrices.insert(pair<Base3DParticleInstance*, D3DXMATRIX>(bbDists[p].idx, bbDists[p].idx->world));
	}

	clouds->CalcVertexBuffer();
}
