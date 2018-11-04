#pragma once
#include "Camera.h"

class OrbitCamera : public Camera
{
public:
	OrbitCamera(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);
	~OrbitCamera();

	void Update();

	void SetTarget(D3DXVECTOR3* targetPos) { this->targetPos = targetPos; }

private:
	bool firstUpdate;
	float moveSpeed;
	float rotationSpeed;

	D3DXVECTOR3 initPos;
	float tpsDistance;
	D3DXVECTOR3 tpsRotation;
	D3DXVECTOR3* targetPos;
};