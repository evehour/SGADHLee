#pragma once
#include "../Components/Collider.h"

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider();
	~CapsuleCollider();

	void Update() override;
	void CreateData();
	void UpdateData();

	void SetHeight(float height) { this->height = height; }
	float GetHeight() { return this->height; }

	void SetRadius(float radius) { this->radius = radius; }
	float GetRadius() { return this->radius; }

	void Scale(float x, float y, float z) { Scale(D3DXVECTOR3(x, y, z)); }
	void Scale(D3DXVECTOR3& val);

	bool IsCollision(Collider* other);
	bool IsCollisionOtherCapsule(CapsuleCollider* other);
	bool IsCollisionRay(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir);
private:
	int band;
	float height;
	float radius;
};