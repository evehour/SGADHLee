#pragma once

class Terrain
{
public:
	struct InitDesc;

public:
	Terrain(InitDesc& desc);
	~Terrain();

	void Initialize();
	void Ready(Material* material);
	void Update();
	void Render(UINT pass = 0U);
	void ResizeScreen();

	void Data(UINT row, UINT col, float data);
	float Data(UINT row, UINT col);

	float Width();
	float Depth();
	float GetHeight(float x, float z);
	bool Picking(OUT D3DXVECTOR3* pickPixel, OUT D3DXVECTOR3* pickWorldPos);

	class HeightMap* GetHeightMap() { return heightMap; }
	ID3D11Texture2D* GetHeightMapTexture();
	ID3D11ShaderResourceView* GetHeightMapSRV();
	ID3D11UnorderedAccessView* GetHeightMapUAV();
	Shader* GetShader() { return desc.material->GetShader(); }

public:
	static const UINT CellsPerPatch;

private:
	class TerrainRender* render;
	class HeightMap* heightMap;

private:
	Shader* shaderCS;
	class CsResource* mousePickData;
	UINT dispatchX, dispatchY;

public:
	struct InitDesc
	{
		wstring HeightMap;
		float HeightScale; // 높이에 대한 비율값
		float HeightMapWidth;
		float HeightMapHeight;

		wstring layerMapFile[5];
		wstring blendMapFile;
		float CellSpacing; // 한 셀의 크기

		Material* material;

		// 랜덤 지형의 쓰는 것
		//float NoiseSize1;
		//float NoiseSize2;
		//float Persistence1;
		//float Persistence2;

		//int Overcast1;
		//int Overcast2;
		//int Seed;
	};
private:
	InitDesc desc;

public:
	InitDesc& Desc() { return desc; }
};