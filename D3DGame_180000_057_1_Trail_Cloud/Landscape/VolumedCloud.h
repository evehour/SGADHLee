#pragma once

class VolumedCloud
{
public:
	enum class SkyType
	{
		SpotClouds,
		CloudySky,
		CloudField,
		CloudSplatter
	};

public:
	VolumedCloud(wstring shaderFile, wstring Texture, SkyType skyType);
	~VolumedCloud();

	void Update();
	void Render();
	

private:
	void AddCloud(int whispCount, D3DXVECTOR3 position, float size, D3DXVECTOR3 min, D3DXVECTOR3 max, float colorMod, vector<int> whispRange);
	void AddCloud(int whispCount, D3DXVECTOR3 position, float size, float radius, float colorMod, vector<int> whispRange);

private:
	class Base3DParticleInstancer* clouds;
	vector<class Base3DParticleInstance *> whisps;
};