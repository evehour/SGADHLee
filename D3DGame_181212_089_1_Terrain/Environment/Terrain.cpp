#include "stdafx.h"
#include "Terrain.h"

#include "HeightMap.h"
#include "TerrainRender.h"

const UINT Terrain::CellsPerPatch = 64;

Terrain::Terrain(InitDesc& desc)
	: desc(desc)
{
	render = new TerrainRender(desc.material, this);

	heightMap = new HeightMap(desc.HeightMapWidth, desc.HeightMapHeight, desc.HeightScale);
	heightMap->Load(desc.HeightMapFileName);

	// ����Ʈ���� Ÿ�ϸ����� �ϴ°� ����. �� ������ Ÿ�ϸ����� �Ǿ�����.
	InitTileMap();

	render->Initialize();
}

Terrain::~Terrain()
{

}

void Terrain::Update()
{

}

void Terrain::Render()
{
	render->Render();
}

void Terrain::Data(UINT row, UINT col, float data)
{
	heightMap->Data(row, col, data);
}

float Terrain::Data(UINT row, UINT col)
{
	return heightMap->Data(row, col);
}

void Terrain::InitTileMap()
{
	ResetTileMap();
	SetTimePositionAndTypes();
	ConnectNeighboringTiles();
	CreateTileSets();
}

void Terrain::ResetTileMap()
{
}

void Terrain::SetTimePositionAndTypes()
{
}

void Terrain::ConnectNeighboringTiles()
{
}

void Terrain::CreateTileSets()
{
}

float Terrain::Width()
{
	return (desc.HeightMapWidth - 1) * desc.CellSpacing;
}

float Terrain::Depth()
{
	return (desc.HeightMapHeight - 1) * desc.CellSpacing;
}
