#pragma once

class Unit
{
public:
	Unit(struct MapTile* mapTile, class PathFinding* pathFinding);
	~Unit();

	void Update();
	void Render();

public:
	static const float HeightOffset;

private:
	void Goto(MapTile* tile);
	void MoveUnit(MapTile* to);

private:
	Material* material;
	MeshSphere* sphere;

	struct MapTile* mapTile;
	vector<struct MapTile *> pathes;

	D3DXVECTOR3 lastWP, nextWP;
	int activeWP;

	D3DXVECTOR3 position;
	class PathFinding* pathFinding;

	bool bMoving;
	float movePrc;
	float time;
	float speed;

};