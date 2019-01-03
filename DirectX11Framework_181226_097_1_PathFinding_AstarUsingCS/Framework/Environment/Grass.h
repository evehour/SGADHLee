#pragma once

class Terrain;
class Grass
{
public:
	struct InstData;
	struct tmpStruct;

public:
	Grass();
	~Grass();

	void Initialize();
	void Ready(Terrain* terrain);
	void Update();
	void Render(UINT pass = 0U);
	void Destroy();

	void AddGrassArea(const D3DXVECTOR3& minPos, const D3DXVECTOR3& maxPos, const UINT count, const InstData& initData);
	void AddGrass(vector<InstData>& package);
	void SetGrass(vector<InstData>& package);

	void AddGrassTexture(const wstring& file);
	void SetGrassTexture(const vector<wstring>& files);

	void GetInstanceData(OUT vector<InstData>& datas);

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

	vector<InstData> grassStorage;
	vector<wstring> textureStorage;

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
			: Position(0, 0, 0), Scale(1, 1)
			, Type(0), ShakeType(0), WindLimitFactor(0.0f)
		{}
	};
};