#pragma once

class GameModel;

class Simplex
{
public:
	Simplex() {};
	~Simplex() {};

	D3DXVECTOR3& operator[](UINT idx) { return points[idx]; }

	UINT GetSize(){ return points.size(); }
	void Add(D3DXVECTOR3 point) { points.push_back(point); }
	void Remove(UINT at) { points.erase(points.begin() + at); }
	D3DXVECTOR3 Get(UINT at) { return points[at]; }
	D3DXVECTOR3 Last() { return points.back(); }
private:
	std::vector<D3DXVECTOR3> points;
};

class Collision
{
public:

	//AABB Section
	static bool ShapeCollisionDetectAuto(
		GameModel& gmA,
		GameModel& gmB
	);

	static bool IntersectRaySphere
	(
		D3DXVECTOR3 rayOrigin,
		D3DXVECTOR3 rayDir,
		D3DXVECTOR3 sphereOrigin,
		float sphereRadius
	);

	static bool IntersectRectToCircle(
		GameModel& gmA,
		GameModel& gmB
	);

	static bool IntersectRectToRect(
		GameModel& gmA,
		GameModel& gmB
	);

	static bool IntersectCircleToCircle(
		GameModel& gmA,
		GameModel& gmB
	);

	static float ClosestPtSegmentSegment(
		const D3DXVECTOR3& p1, const D3DXVECTOR3& q1, const D3DXVECTOR3& p2, const D3DXVECTOR3& q2,
		float& s, float& t, D3DXVECTOR3& c1, D3DXVECTOR3& c2
	);
};