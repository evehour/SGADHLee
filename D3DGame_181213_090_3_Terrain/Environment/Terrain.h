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

	void Data(UINT row, UINT col, float data);
	float Data(UINT row, UINT col);

	float Width();
	float Depth();

	class HeightMap* GetHeightMap() { return heightMap; }

private:
	
#pragma region Pathfinding
	void InitTileMap();
	void ResetTileMap();
	void SetTilePositionAndTypes();
	//void CalculateWalkability();
	void ConnectNeighboringTiles();
	void CreateTileSets();
#pragma endregion



public:
	static const UINT CellsPerPatch;

private:
	class TerrainRender* render;
	class HeightMap* heightMap;

public:
	struct InitDesc
	{
		wstring HeightMap;
		float HeightScale; // ���̿� ���� ������
		float HeightMapWidth;
		float HeightMapHeight;

		wstring layerMapFile[5];
		wstring blendMapFile;
		float CellSpacing; // �� ���� ũ��

		Material* material;

		// ���� ������ ���� ��
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