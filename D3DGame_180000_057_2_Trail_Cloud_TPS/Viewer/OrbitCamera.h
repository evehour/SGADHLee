#pragma once
#include "Camera.h"

class OrbitCamera : public Camera
{
public:
	OrbitCamera(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);
	~OrbitCamera();

	void Update();

	void SetTarget(D3DXVECTOR3* targetPos) { this->targetPos = targetPos; }

	void SetRotation(D3DXVECTOR2 angle);
	void SetDeltaRotation(D3DXVECTOR2 delta);

private:
	bool firstUpdate;
	float moveSpeed;
	float rotationSpeed;
	float orbitRotationX;
	float orbitTheta;
	float orbitPhi;

	float tpsDistance;

	D3DXVECTOR3 initPos;

	D3DXVECTOR3 tpsRotation;
	D3DXVECTOR3* targetPos;

	void SetRotationHelper();
};