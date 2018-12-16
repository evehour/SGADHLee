#pragma once

struct MapEdge;

//우선순위 큐는 힙으로 만듦.
struct MapTile
{
	const UINT EdgeCount = 8;

	int Type;
	bool Walkable;
	int Set;

	float F; // 비용
	float G; // goal

	MapTile* Parent();
	MapEdge* Edges;

	Point MapPosition;
	D3DXVECTOR3 WorldPosition;

	float Height() { return WorldPosition.y; }
	static float MaxSlope() { return 0.6f; }

	MapTile()
	{
		Edges = NULL;
	}
	~MapTile()
	{
		SAFE_DELETE_ARRAY(Edges);
	}
};

struct MapEdge
{
	MapTile* Node1;
	MapTile* Node2;
	float Cost;

private:
	MapEdge(MapTile* n1, MapTile* n2, float cost)
	{
		Node1 = n1;
		Node2 = n2;
		Cost = cost;
	}

	static float CalculateCost(MapTile* n1, MapTile* n2)
	{
		float dx = fabsf(n1->WorldPosition.x - n2->WorldPosition.x);
		float dy = fabsf(n1->WorldPosition.y - n2->WorldPosition.y);
		float dz = fabsf(n1->WorldPosition.z - n2->WorldPosition.z);

		float dxz = sqrtf(dx * dx + dz * dz);
		float slope = dy / dxz;

		return 1.0f + slope;
	}

public:
	static MapEdge* Create(MapTile* tile, MapTile* neighbor)
	{
		float cost = CalculateCost(tile, neighbor);
		if (cost < MapTile::MaxSlope() + 1.0f)
			return new MapEdge(tile, neighbor, cost);

		return NULL;
	}
};

class PathFinding
{
public:
	PathFinding(class Terrain* terrain, UINT tileSize = 2);
	~PathFinding();

private:
	void ResetTileMap();
	void SetTilePositionAndTypes();
	void CalulateWalkAbility();
	void ConnectNeighboringTiles();
	void CreateTileSets();

private:
	class Terrain* terrain;
	UINT tileSize;

	UINT widthInTiles;
	UINT heightInTiles;

	MapTile* tiles;

public:
	MapTile* GetTile(int x, int y);
};