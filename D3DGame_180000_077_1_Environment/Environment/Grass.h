#pragma once

class Terrain;
class Grass
{
public:
	struct InstData;
	struct tmpStruct;

public:
	Grass(Terrain* terrain);
	~Grass();

	void Update();
	void Render();

private:
	int drawCount;

private:
	D3DXVECTOR2 WindDirection;
	float DeltaSTime;
	float ShakeFactor;

	UINT grassCount;
	Terrain* terrain;

	Shader* shader;
	TextureArray* textureArray;
	ID3D11Buffer* vertexBuffer[2];

	InstData* intanceData;
	tmpStruct* tmps;

public:
	struct InstData
	{
		D3DXMATRIX World;
		UINT Type;
		UINT ShakeType;
		float WindLimitFactor;
	};

	struct tmpStruct
	{
		D3DXVECTOR2 Scale;
		D3DXVECTOR3 Position;
	};

	static bool acompare(InstData a, InstData b);
};