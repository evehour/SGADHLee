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

	UINT grassCount, MaximumGrassCount;
	Terrain* terrain;

	Shader* shader;
	TextureArray* textureArray;
	ID3D11Buffer* vertexBuffer[2];

	InstData *intanceData;
	vector<InstData> chkval;

private:
	const UINT BLOCK_SIZE = 512;
	const UINT NUM_ELEMENTS = BLOCK_SIZE * BLOCK_SIZE;
	const UINT MATRIX_WIDTH = BLOCK_SIZE;
	const UINT MATRIX_HEIGHT = NUM_ELEMENTS / BLOCK_SIZE;
	const UINT TRANSPOSE_BLOCK_SIZE = 16;

	ID3D11Buffer*               pBuffer1 = nullptr;
	ID3D11ShaderResourceView*   pBuffer1SRV = nullptr;
	ID3D11UnorderedAccessView*  pBuffer1UAV = nullptr;
	ID3D11Buffer*               pBuffer2 = nullptr;
	ID3D11ShaderResourceView*   pBuffer2SRV = nullptr;
	ID3D11UnorderedAccessView*  pBuffer2UAV = nullptr;

	ID3D11Buffer*               pReadBackBuffer = nullptr;

public:
	struct InstData
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR2 Scale;
		UINT Type;
		UINT ShakeType;
		float WindLimitFactor;

		InstData()
		{
			Position = { 0,0,0 };
			Scale = { 0,0 };
			Type = 0;
			ShakeType = 0;
			WindLimitFactor = 0.0f;
		}
	};

	static bool acompare(InstData a, InstData b);
};