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
	// ��ã��
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

		float CellSpacing; // �� ���� ũ��
		Material* material;

		// �� �Ʒ��� ������ �������� ����� ���� �ִ� �͵�
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