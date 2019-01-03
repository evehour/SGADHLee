#include "Framework.h"
#include "Transform.h"

Transform::Transform(GameObject * owner)
	: IComponent(owner)
	, localScale(1, 1, 1)
	, localPosition(0, 0, 0)
	, localQuaternion(0, 0, 0, 0)
	, parent(NULL)
{
	D3DXMatrixIdentity(&local);
	D3DXMatrixIdentity(&world);
	bEnabled = true;
}

Transform::~Transform()
{
}

const D3DXVECTOR3 Transform::GetWorldScale() const
{
	D3DXVECTOR3 scale(1, 1, 1);
	D3DXMatrixDecompose(&scale, &D3DXQUATERNION(), &D3DXVECTOR3(), &world);
	return scale;
}

const D3DXVECTOR3 Transform::GetWorldPosition() const
{
	return D3DXVECTOR3(world._41, world._42, world._43);
}

const D3DXQUATERNION Transform::GetWorldRotate() const
{
	D3DXQUATERNION rotate(0, 0, 0, 1);
	D3DXMatrixDecompose(&D3DXVECTOR3(), &rotate, &D3DXVECTOR3(), &world);
	return rotate;
}

const D3DXVECTOR3 Transform::GetEulerAngle() const
{
	D3DXQUATERNION qut = GetWorldRotate();
	D3DXVECTOR3 angle;
	Math::toEulerAngle(qut, angle);

	return angle;
}

const D3DXVECTOR3 Transform::Right()
{
	return right;
}

const D3DXVECTOR3 Transform::Up()
{
	return up;
}

const D3DXVECTOR3 Transform::Forward()
{
	return forward;
}

const D3DXMATRIX Transform::GetWorldRotationMatrix() const
{
	D3DXMATRIX matRotation;
	D3DXQUATERNION qut;
	D3DXMatrixDecompose(&D3DXVECTOR3(), &qut, &D3DXVECTOR3(), &world);
	D3DXMatrixRotationQuaternion(&matRotation, &qut);

	return matRotation;
}

Transform * Transform::GetChildFromIndex(const UINT & index)
{
	if (!HasChilds())
		return nullptr;

	if (index >= GetChildsCount())
		return nullptr;

	return childs[index];
}

void Transform::SetLocalScale(const D3DXVECTOR3 & vec)
{
	if (localScale == vec)
		return;

	localScale = vec;
	UpdateTransform();
}

void Transform::SetLocalPosition(const D3DXVECTOR3 & vec)
{
	if (localPosition == vec)
		return;

	localPosition = vec;
	UpdateTransform();
}

void Transform::SetLocalRotate(const D3DXQUATERNION & vec)
{
	if (localQuaternion == vec)
		return;

	localQuaternion = vec;
	UpdateTransform();
}

void Transform::SetWorldScale(const D3DXVECTOR3 & vec)
{
	if (GetWorldScale() == vec)
		return;

	if (HasParent())
	{
		D3DXVECTOR3 parentScale = parent->GetWorldScale();
		D3DXVECTOR3 scale;
		scale.x = vec.x / parentScale.x;
		scale.y = vec.y / parentScale.y;
		scale.z = vec.z / parentScale.z;

		SetLocalScale(scale);
	}
	else
	{
		SetLocalScale(vec);
	}
}

void Transform::SetWorldPosition(const D3DXVECTOR3 & vec)
{
	if (GetWorldPosition() == vec)
		return;

	if (HasParent())
	{
		D3DXMATRIX inv;
		D3DXMatrixInverse(&inv, nullptr, &parent->GetWorldMatrix());

		D3DXVECTOR3 position;
		D3DXVec3TransformCoord(&position, &vec, &inv);

		SetLocalPosition(position);
	}
	else
		SetLocalPosition(vec);
}

void Transform::SetWorldRotate(const D3DXQUATERNION & qut)
{
	if (GetWorldRotate() == qut)
		return;

	if (HasParent())
	{
		D3DXQUATERNION inv;
		D3DXQuaternionInverse(&inv, &parent->GetWorldRotate());

		D3DXQUATERNION quaternion = qut * inv;
		SetLocalRotate(quaternion);
	}
	else
		SetLocalRotate(qut);
}

void Transform::SetWorldMatrix(const D3DXMATRIX & mat)
{
	D3DXVECTOR3 scale, transform;
	D3DXQUATERNION quaternion;
	D3DXMatrixDecompose(&scale, &quaternion, &transform, &mat);

	SetWorldScale(scale);
	SetWorldRotate(quaternion);
	SetWorldPosition(transform);
}

void Transform::SetParent(Transform * newParent)
{
	if (newParent == this)
		return;

	if (HasParent())
	{
		if (parent == newParent)
			return;

		parent->DelChild(this);
	}

	parent = newParent;

	UpdateTransform();
}

void Transform::DelParent()
{
	if (!HasParent())
		return;

	parent->DelChild(this);
	parent = NULL;

	UpdateTransform();
}

void Transform::AddChild(Transform * child)
{
	if (!child)
		return;

	if (child == this)
		return;

	child->SetParent(this);
	childs.push_back(child);
}

void Transform::DelChild(Transform * child)
{
	for (vector<Transform*>::iterator it = childs.begin(); it != childs.end();)
	{
		if (*it == child)
		{
			(*it)->SetParent(NULL);
			childs.erase(it);
			break;
		}
	}
}

bool Transform::IsChild(Transform * transform)
{
	vector<Transform*> tempChilds = transform->GetChilds();

	for (Transform* child : tempChilds)
	{
		if (child == this)
			return true;
	}
	return false;
}

void Transform::UpdateTransform()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, localScale.x, localScale.y, localScale.z);
	D3DXMatrixRotationQuaternion(&R, &localQuaternion);
	D3DXMatrixTranslation(&T, localPosition.x, localPosition.y, localPosition.z);

	local = S * R * T;

	if (HasParent())
		world = local * parent->GetWorldMatrix();
	else
		world = local;

	right = D3DXVECTOR3(world._11, world._12, world._13);
	D3DXVec3Normalize(&right, &right);

	up = D3DXVECTOR3(world._21, world._22, world._23);
	D3DXVec3Normalize(&up, &right);

	forward = D3DXVECTOR3(world._31, world._32, world._33);
	D3DXVec3Normalize(&right, &right);

	for (const auto& child : childs)
		child->UpdateTransform();
}
