#pragma once
#include "Interfaces/IComponent.h"

class Transform : public IComponent
{
public:
	Transform(class GameObject* owner);
	virtual ~Transform();

	const D3DXVECTOR3& GetLocalScale() const { return localScale; }
	const D3DXVECTOR3& GetLocalPosition() const { return localPosition; }
	const D3DXQUATERNION& GetLocalRotate() const { return localQuaternion; }

	const D3DXVECTOR3 GetWorldScale() const;
	const D3DXVECTOR3 GetWorldPosition() const;
	const D3DXQUATERNION GetWorldRotate() const;
	const D3DXVECTOR3 GetEulerAngle() const;

	const D3DXVECTOR3 Right();
	const D3DXVECTOR3 Up();
	const D3DXVECTOR3 Forward();

	const D3DXMATRIX& GetLocalMatrix() const { return local; }
	const D3DXMATRIX& GetWorldMatrix() const { return world; }
	const D3DXMATRIX GetWorldRotationMatrix() const;

	Transform* GetRoot() { return HasParent() ? parent->GetRoot() : this; }
	Transform* GetParent() { return parent; }
	Transform* GetChildFromIndex(const UINT& index);
	const vector<Transform*>& GetChilds() const { return childs; }
	const UINT GetChildsCount() const { return childs.size(); }

	void SetLocalScale(const D3DXVECTOR3& vec);
	void SetLocalPosition(const D3DXVECTOR3& vec);
	void SetLocalRotate(const D3DXQUATERNION& vec);

	void SetWorldScale(const D3DXVECTOR3& vec);
	void SetWorldPosition(const D3DXVECTOR3& vec);
	void SetWorldRotate(const D3DXQUATERNION& qut);

	void SetWorldMatrix(const D3DXMATRIX& mat);

	void SetParent(Transform* newParent);
	void DelParent();

	void AddChild(Transform* child);
	void DelChild(Transform* child);

	bool HasParent() { return parent ? true : false; }
	bool HasChilds() { return !childs.empty(); }
	bool IsChild(Transform* transform);


	// IComponent을(를) 통해 상속됨
	void Initialize() override {}
	void Destroy() override {}
	void Ready() override {}
	void PreRender() override {}
	void Render() override {}
	void PostRender() override {}
	void Update() override {}
	void UpdateTransform();

private:
	class ConstantBuffer* transformBuffer;

	D3DXVECTOR3 localScale;
	D3DXVECTOR3 localPosition;
	D3DXQUATERNION localQuaternion;

	D3DXMATRIX local;
	D3DXMATRIX world;

	Transform* parent;
	vector<Transform*> childs;

	D3DXVECTOR3 right;
	D3DXVECTOR3 up;
	D3DXVECTOR3 forward;
};