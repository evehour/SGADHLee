#pragma once
#include "Utilities/PriorityQueue.h"

struct MapTile;

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

	static float CalculateCost(MapTile* n1, MapTile* n2);

public:
	static MapEdge* Create(MapTile* tile, MapTile* neighbor);
};

//우선순위 큐는 힙으로 만듦.
struct MapTile : public IPriorityNode
{
	int Type;
	bool Walkable;
	int Set;

	float F; // 비용
	float G; // goal

	MapTile* Parent;
	MapEdge* Edges[8];

	Point MapPosition;
	D3DXVECTOR3 WorldPosition;

	float Height() { return WorldPosition.y; }
	static const float MaxSlope;
	static const UINT EdgeCount;

	MapTile()
	{
		for (UINT i = 0; i < EdgeCount; i++)
			Edges[i] = NULL;
	}
	~MapTile()
	{
		for (UINT i = 0; i < EdgeCount; i++)
			SAFE_DELETE(Edges[i]);
	}

	bool operator<(const MapTile& a)
	{
		return this->F < a.F;
	}


	virtual float Priority() override;
	virtual void Priority(float val) override;
	virtual int InsertionIndex() override;
	virtual void InsertionIndex(int val) override;
	virtual int QueueIndex() override;
	virtual void QueueIndex(int val) override;

private:
	float priority = 0;
	int insertionIndex = 0;
	int queueIndex = 0;
};

class PathFinding
{
public:
	PathFinding(class Terrain* terrain, UINT tileSize = 2);
	~PathFinding();

	void GetPath(vector<MapTile*>& tiles, Point& start, Point& goal);

private:
	void ResetTileMap();
	void SetTilePositionAndTypes();
	void CalulateWalkAbility();
	void ConnectNeighboringTiles();
	void CreateTileSets();

private:
	class Terrain* terrain;
	UINT tileSize;

	int widthInTiles;
	int heightInTiles;

	MapTile* tiles;

public:
	MapTile* GetTile(Point point);
	MapTile* GetTile(int x, int y);
	MapTile* GetTile(D3DXVECTOR2 pos);
	
	class Terrain* GetTerrain();

private:
	bool Within(Point p);
};