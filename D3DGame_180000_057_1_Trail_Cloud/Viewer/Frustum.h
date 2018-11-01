#pragma once

class Frustum
{
public:
	Frustum(ExecuteValues* values, float zFar, Camera* camera = NULL, Perspective* perspective = NULL);
	~Frustum();

	void Update();
	void Render();

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

	//Frustum area box
	class LineMake* lineMake;
	D3DXVECTOR3 frustumArea[8];

private:
	D3DXVECTOR3 IntersectRayPlane(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDirection, const D3DXPLANE& plane, OUT D3DXVECTOR3* position = NULL);
	void IntersectPlanePlane(const D3DXPLANE& p1, const D3DXPLANE& p2, OUT D3DXVECTOR3& rayOrigin, OUT D3DXVECTOR3& rayDirection);

public:
	void SetPickingArea(const D3DXVECTOR3& start, const D3DXVECTOR3& end, D3DXVECTOR3* const arr = NULL);
	void GetFrustumArea(D3DXVECTOR3* arr);
};