#pragma once

class Terrain
{
public:
	struct InitDesc;

public:
	Terrain(InitDesc& desc);
	~Terrain();

	void Update();
	void Render();

	static const UINT CellsPerPatch;

	float Width();
	float Depth();
	class HeightMap* GetHeightMap() { return heightMap; }

	void Data(UINT row, UINT col, float data);
	float Data(UINT row, UINT col);

private:
	// 길찾기
	void InitTileMap();
	void ResetTileMap();
	void SetTimePositionAndTypes();
	void ConnectNeighboringTiles();
	void CreateTileSets();

private:
	class TerrainRender* render;
	class HeightMap* heightMap;

public:
	struct InitDesc
	{
		wstring HeightMapFileName;
		float HeightScale;
		float HeightMapWidth;
		float HeightMapHeight;

		wstring LayerMapFile[5];
		wstring BlendMapFile;

		float CellSpacing; // 한 셀의 크기
		Material* material;

		// 이 아래는 지면을 랜덤으로 만들기 위해 있는 것들
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