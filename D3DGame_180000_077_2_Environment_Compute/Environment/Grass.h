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

	InstData *intanceData;
	vector<InstData> chkval;

private:
	UINT BLOCK_SIZE = 512;
	UINT MaximumGrassCount = BLOCK_SIZE * BLOCK_SIZE;

	ID3D11Buffer*               pBuffer = nullptr;
	ID3D11UnorderedAccessView*  pBufferUAV = nullptr;
	ID3D11Buffer*               pReadBackBuffer = nullptr;

public:
	struct InstData
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR2 Scale;
		UINT Type;
		UINT ShakeType;
		float WindLimitFactor;
	};

	static bool acompare(InstData a, InstData b);
};