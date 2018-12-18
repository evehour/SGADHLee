#include "stdafx.h"
#include "PathFinding.h"

#include "Terrain.h"
#include "HeightMap.h"

const float MapTile::MaxSlope = 0.6f;
const UINT MapTile::EdgeCount = 8;

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

	for (int y = 0; y < heightInTiles; y++)
	{
		for (int x = 0; x < widthInTiles; x++)
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

// Terrain.cs
void PathFinding::CalulateWalkAbility()
{
	for (int y = 0; y < (int)heightInTiles; y++)
	{
		for (int x = 0; x < (int)widthInTiles; x++)
		{
			MapTile* tile = GetTile(x, y);
			if (tile == NULL) continue;

			Point p[8] = {
				Point(x - 1, y - 1), Point(x, y - 1), Point(x + 1, y - 1),
				Point(x - 1, y), Point(x + 1, y),
				Point(x - 1, y + 1), Point(x, y + 1), Point(x + 1, y + 1),
			};

			float variance = 0.0f;
			int nr = 0;
			for (Point point : p)
			{
				if (!Within(point))
					continue;

				MapTile* neighbor = GetTile(point);
				if (neighbor == NULL)
					continue;

				float v = neighbor->Height() - tile->Height();

				// ignore neighbors on the same plane as this tile
				if (v <= 0.01f)
					continue;

				variance += v * v;
				nr++;
			}

			//prevent divide by 0
			if (nr == 0) nr = 1;
			variance /= nr;

			tile->Walkable = variance < MapTile::MaxSlope;
		}
	}
}

void PathFinding::ConnectNeighboringTiles()
{
	Terrain::InitDesc desc = terrain->Desc();

	for (int y = 0; y < heightInTiles; y++)
	{
		for (int x = 0; x < widthInTiles; x++)
		{
			MapTile* tile = GetTile(x, y);
			if (tile == NULL || !tile->Walkable)
				continue;

			Point p[8] = {
				Point(x - 1, y - 1), Point(x, y - 1), Point(x + 1, y - 1),
				Point(x - 1, y), Point(x + 1, y),
				Point(x - 1, y + 1), Point(x, y + 1), Point(x + 1, y + 1),
			};

			for (UINT i = 0; i < tile->EdgeCount; i++)
			{
				Point point = p[i];
				if (!Within(point))
					continue;

				MapTile* neighbor = GetTile(point);
				if (neighbor != NULL && neighbor->Walkable)
				{
					tile->Edges[i] = MapEdge::Create(tile, neighbor);
				}
			}
		}//for(x)
	}//for(y)
}

void PathFinding::CreateTileSets()
{
	int setNo = 0;
	
	unordered_set<MapTile*> unVisited;
	for (int y = 0; y < heightInTiles; y++)
	{
		for (int x = 0; x < widthInTiles; x++)
		{
			MapTile* tile = GetTile(x, y);
			for (UINT i = 0; i < MapTile::EdgeCount; i++)
			{
				if (tile->Edges[i] != NULL)
				{
					if (tile->Walkable)
						unVisited.insert(tile);
					else
						tile->Set = --setNo;
				}
				else
				{
					tile->Set = --setNo;
				}
			}//for(i)
		}//for(x)
	}//for(y)

	setNo = 0;
	stack<MapTile*> tempStack;

	while (unVisited.empty() == false)
	{
		unordered_set<MapTile*>::iterator newFirst = unVisited.begin();
		tempStack.push((*newFirst));
		unVisited.erase(newFirst);

		while (tempStack.empty() == false)
		{
			MapTile* next = tempStack.top();
			tempStack.pop();

			next->Set = setNo;

			for (UINT i = 0; i < MapTile::EdgeCount; i++)
			{
				if (next->Edges[i] != NULL && unVisited.find(next->Edges[i]->Node2) != unVisited.end())
				{
					tempStack.push(next->Edges[i]->Node2);
					unVisited.erase(next->Edges[i]->Node2);
				}
			}//for(i)
		}//while(tempStack)

		setNo++;
	}//while(unVisited)
}

MapTile * PathFinding::GetTile(Point point)
{
	return GetTile(point.X, point.Y);
}

MapTile* PathFinding::GetTile(int x, int y)
{
	if (x < 0 || x >= widthInTiles || y < 0 || y >= heightInTiles)
		return NULL;

	return &tiles[x + y * heightInTiles];
}

bool PathFinding::Within(Point p) {
	return p.X >= 0 && p.X < widthInTiles && p.Y >= 0 && p.Y < heightInTiles;
}

inline float MapEdge::CalculateCost(MapTile * n1, MapTile * n2)
{
	float dx = fabsf(n1->WorldPosition.x - n2->WorldPosition.x);
	float dy = fabsf(n1->WorldPosition.y - n2->WorldPosition.y);
	float dz = fabsf(n1->WorldPosition.z - n2->WorldPosition.z);

	float dxz = sqrtf(dx * dx + dz * dz);
	float slope = dy / dxz;

	return 1.0f + slope;
}

inline MapEdge * MapEdge::Create(MapTile * tile, MapTile * neighbor)
{
	float cost = CalculateCost(tile, neighbor);
	if (cost < MapTile::MaxSlope + 1.0f)
		return new MapEdge(tile, neighbor, cost);

	return NULL;
}
