#pragma once
#include "Component.h"

class Transform : public Component
{
public:
	enum TRANSFORM_RESET_FLAG
	{
		TRANSFORM_RESET_FLAG_SCALE = 1,
		TRANSFORM_RESET_FLAG_ROTATION = 2,
		TRANSFORM_RESET_FLAG_POSITION = 4
	};

protected:
	D3DXMATRIX matLocal;
	D3DXMATRIX matWorld;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	union
	{
		struct
		{
			D3DXVECTOR3 axis[3];
		};
		struct
		{
			D3DXVECTOR3 right;
			D3DXVECTOR3 up;
			D3DXVECTOR3 forward;
		};
	}; // Local

	bool bAutoUpdate;

	Transform* pParent;
	Transform* pFirstChild; //첫번째 자식
	Transform* pNextSibling; //형제

public:
	Transform();
	~Transform();

	void Reset(int resetFlag = -1);

	void AddChild(Transform* pNewChild);
	void SetParent(Transform* parent);
	void ReleaseParent();

	void SetWorldPosition(const D3DXVECTOR3& position);
	void SetLocalPosition(const D3DXVECTOR3& position);
	void MoveLocalPosition(const D3DXVECTOR3& deltaPosition);
	void MoveWorldPosition(const D3DXVECTOR3& deltaPosition);

	void SetScale(const D3DXVECTOR3& scale);
	void SetScaling(const D3DXVECTOR3& deltaScale);

	void SetWorldRotation(const D3DXVECTOR3& rotation);
	void SetWorldRotation(const float& x, const float& y, const float& z);
	void SetLocalRotation(const D3DXVECTOR3& rotation);
	void SetLocalRotation(const float& x, const float& y, const float& z);

	void RotationLocal(const D3DXVECTOR3& deltaRotation);

	D3DXMATRIX World() const;
	D3DXMATRIX Local() const;

	D3DXVECTOR3 GetWorldPosition() const;
	D3DXVECTOR3 GetLocalPosition() const;

	D3DXVECTOR3 GetScale() const;

	void GetAxis(D3DXVECTOR3* pArr) const;
	D3DXVECTOR3 GetAxis(const UINT& i) const;
	D3DXVECTOR3 GetAngle() const;

	void UpdateTransform();

private:
	void toEulerAngle(const D3DXQUATERNION& q, double& pitch, double& yaw, double& roll);
};