#pragma once

class Frustum
{
public:
	Frustum(ExecuteValues* values, float zFar, Camera* camera = NULL, Perspective* perspective = NULL);
	~Frustum();

	void Update();

	bool ContainPoint(D3DXVECTOR3& position, const int & cases = 0);
	bool ContainCube(D3DXVECTOR3& center, const float & radius, const int & cases = 0);

private:
	ExecuteValues* values;

	D3DXPLANE planes[6];
	
	float zFar;
	
	Camera* camera;
	Perspective* perspective;

private:
	//Multi-Picking Frustum Area
	D3DXPLANE pickAreaPlanes[6];
public:
	void SetPickingArea(const D3DXVECTOR3& start, const D3DXVECTOR3& end, D3DXVECTOR3* const arr = NULL);
};