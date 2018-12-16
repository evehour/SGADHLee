#include "stdafx.h"
#include "PathFinding.h"

#include "Terrain.h"
#include "HeightMap.h"

PathFinding::PathFinding(Terrain* terrain, UINT tileSize)
	: tiles(NULL)
	, terrain(terrain), tileSize(tileSize)
{
	ResetTileMap();
	SetTilePositionAndTypes();
	CalulateWalkAbility();
	ConnectNeighboringTiles();
	CreateTileSets();
}

PathFinding::~PathFinding()
{
	SAFE_DELETE_ARRAY(tiles);
}

void PathFinding::ResetTileMap()
{
	widthInTiles = (UINT)terrain->Desc().HeightMapWidth / tileSize;
	heightInTiles = (UINT)terrain->Desc().HeightMapHeight / tileSize;

	tiles = new MapTile[widthInTiles * heightInTiles];
}

void PathFinding::SetTilePositionAndTypes()
{
	Terrain::InitDesc desc = terrain->Desc();

	for (UINT y = 0; y < heightInTiles; y++)
	{
		for (UINT x = 0; x < widthInTiles; x++)
		{
			MapTile* tile = GetTile(x, y);
			tile->MapPosition = Point(x, y);

			float worldX = (x * desc.CellSpacing * tileSize) + (desc.CellSpacing * tileSize / 2.0f) - terrain->Width() / 2.0f;
			float worldZ = (-y * desc.CellSpacing * tileSize) - (desc.CellSpacing * tileSize / 2.0f) + terrain->Depth() / 2.0f;
			float height = terrain->GetHeight(worldX, worldZ);
			tile->WorldPosition = D3DXVECTOR3(worldX, height, worldZ);

			if (tile->Height() < terrain->GetHeightMap()->MaxHeight() * 0.05f)
			{
				tile->Type = 0;
			}
			else if (tile->Height() < terrain->GetHeightMap()->MaxHeight() * 0.4f)
			{
				tile->Type = 1;
			}
			else if (tile->Height() < terrain->GetHeightMap()->MaxHeight() * 0.75f)
			{
				tile->Type = 2;
			}
			else
			{
				tile->Type = 3;
			}
		}//for(x)
	}//for(y)
}

void PathFinding::CalulateWalkAbility()
{
	for (int y = 0; y < (int)heightInTiles; y++)
	{
		for (int x = 0; x < (int)widthInTiles; x++)
		{
			MapTile* tile = GetTile(x, y);


		}
	}
}

void PathFinding::ConnectNeighboringTiles()
{
}

void PathFinding::CreateTileSets()
{
}

MapTile* PathFinding::GetTile(int x, int y)
{
	return &tiles[x + y * heightInTiles];
}
