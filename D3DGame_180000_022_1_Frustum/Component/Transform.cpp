#include "stdafx.h"
#include "Transform.h"

Transform::Transform()
	:pParent(NULL), pFirstChild(NULL), pNextSibling(NULL)
	, bAutoUpdate(true)
{
	Reset();

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

Transform::~Transform()
{
}

void Transform::Reset(int resetFlag)
{
	if (resetFlag & TRANSFORM_RESET_FLAG_POSITION)
	{
		position = { 0,0,0 };
	}
	if (resetFlag & TRANSFORM_RESET_FLAG_ROTATION)
	{
		right = { 1, 0, 0 };
		up = { 0, 1, 0 };
		forward = { 0, 0, 1 };
		rotation = { 0, 0, 0 };
	}
	if (resetFlag & TRANSFORM_RESET_FLAG_SCALE)
	{
		scale = { 1, 1, 1 };
	}
}

void Transform::AddChild(Transform * pNewChild)
{
	if (pNewChild->pParent == this)
		return;

	//이전 부모와 연결을 끊는다.
	pNewChild->ReleaseParent();

	//부모의 상대적인 좌표값으로 갱신하기 위해서
	//부모의 final 역행렬 값
	D3DXMATRIX matInvWorld;

	D3DXMatrixInverse(&matInvWorld, NULL, &this->matWorld);
	D3DXVec3TransformCoord(&pNewChild->position, &pNewChild->position, &matInvWorld);

	for (int i = 0; i < 3; i++)
		D3DXVec3TransformNormal(&pNewChild->axis[i], &pNewChild->axis[i], &matInvWorld);

	pNewChild->scale = 
	{
		D3DXVec3Length(&pNewChild->right),
		D3DXVec3Length(&pNewChild->up),
		D3DXVec3Length(&pNewChild->forward)
	};

	{
		D3DXMATRIX m;
		D3DXMatrixIdentity(&m);
		m._11 = pNewChild->right.x;		m._12 = pNewChild->right.y;		m._13 = pNewChild->right.z;
		m._21 = pNewChild->up.x;		m._22 = pNewChild->up.y;		m._23 = pNewChild->up.z;
		m._31 = pNewChild->forward.x;	m._32 = pNewChild->forward.y;	m._33 = pNewChild->forward.z;

		D3DXVECTOR3 _s, _r, _t;
		D3DXQUATERNION _qr;
		D3DXMatrixDecompose(&_s, &_qr, &_t, &m);
		double y, p, r;
		toEulerAngle(_qr, p, y, r);
		pNewChild->rotation = { (float)p, (float)y, (float)r };
	}

	pNewChild->pParent = this;

	Transform* pChild = this->pFirstChild;

	if (pChild == NULL)
	{
		this->pFirstChild = pNewChild;
	}
	else
	{
		while (pChild != NULL)
		{
			if (pChild->pNextSibling == NULL)
			{
				pChild->pNextSibling = pNewChild;
				break;
			}
			pChild = pChild->pNextSibling;
		}
	}
	this->UpdateTransform();
}

void Transform::SetParent(Transform * parent)
{
	parent->AddChild(this);
}

void Transform::ReleaseParent()
{
	pParent = NULL;
}

void Transform::SetWorldPosition(const D3DXVECTOR3 & position)
{
	D3DXVECTOR3 _position = position;
	if (pParent)
	{
		D3DXMATRIX matInvParentFinal = pParent->World();
		D3DXMatrixInverse(&matInvParentFinal, NULL, &matInvParentFinal);
		D3DXVec3TransformCoord(&_position, &_position, &matInvParentFinal);
	}

	this->position = _position;

	if (bAutoUpdate)
		UpdateTransform();
}

void Transform::SetLocalPosition(const D3DXVECTOR3 & position)
{
	this->position = position;

	if (bAutoUpdate)
		UpdateTransform();
}

void Transform::MoveLocalPosition(const D3DXVECTOR3 & deltaPosition)
{
	D3DXVECTOR3 pos(0, 0, 0);
	pos = pos + axis[0] * deltaPosition.x;
	pos = pos + axis[1] * deltaPosition.y;
	pos = pos + axis[2] * deltaPosition.z;

	D3DXVECTOR3 nowWorldPos = this->GetWorldPosition();
	SetWorldPosition(nowWorldPos + pos);
}

void Transform::MoveWorldPosition(const D3DXVECTOR3 & deltaPosition)
{
	D3DXVECTOR3 nowWorldPos = this->GetWorldPosition();
	SetWorldPosition(nowWorldPos + deltaPosition);
}

void Transform::SetScale(const D3DXVECTOR3 & scale)
{
	this->scale.x = scale.x;
	this->scale.y = scale.y;
	this->scale.z = scale.z;

	if (bAutoUpdate)
		UpdateTransform();
}

void Transform::SetScaling(const D3DXVECTOR3 & deltaScale)
{
	scale.x += deltaScale.x;
	scale.y += deltaScale.y;
	scale.z += deltaScale.z;

	if (bAutoUpdate)
		UpdateTransform();
}

void Transform::toEulerAngle(const D3DXQUATERNION& q, double& pitch, double& yaw, double& roll)
{
	// pitch (x-axis rotation)
	double sinr = +2.0 * (q.w * q.x + q.y * q.z);
	double cosr = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
	pitch = atan2(sinr, cosr);

	// yaw (y-axis rotation)
	double sinp = +2.0 * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1)
		yaw = copysign(D3DX_PI / 2, sinp); // use 90 degrees if out of range
	else
		yaw = asin(sinp);

	// roll (z-axis rotation)
	double siny = +2.0 * (q.w * q.z + q.x * q.y);
	double cosy = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	roll = atan2(siny, cosy);
}

void Transform::SetWorldRotation(const D3DXVECTOR3 & rotation)
{
	if (pParent)
	{
		D3DXQUATERNION quatRot;
		D3DXQuaternionRotationYawPitchRoll(&quatRot, rotation.y, rotation.x, rotation.z);

		D3DXMATRIX matRotate;
		D3DXMatrixRotationQuaternion(&matRotate, &quatRot);

		D3DXMATRIX matInvParentFinal;
		D3DXMatrixInverse(&matInvParentFinal, NULL, &pParent->World());

		matRotate = matRotate * matInvParentFinal;

		D3DXVECTOR3 _s, _r, _t;
		D3DXQUATERNION _qr;
		D3DXMatrixDecompose(&_s, &_qr, &_t, &matRotate);
		double y, p, r;
		toEulerAngle(_qr, p, y, r);
		this->rotation.x = (float)p;
		this->rotation.y = (float)y;
		this->rotation.z = (float)r;

		this->axis[0] = D3DXVECTOR3(matRotate._11, matRotate._12, matRotate._13);
		this->axis[1] = D3DXVECTOR3(matRotate._21, matRotate._22, matRotate._23);
		this->axis[2] = D3DXVECTOR3(matRotate._31, matRotate._32, matRotate._33);

		if (bAutoUpdate)
			UpdateTransform();
	}
	else
	{
		SetLocalRotation(rotation);
	}
}

void Transform::SetWorldRotation(const float & x, const float & y, const float & z)
{
	D3DXVECTOR3 rot(x, y, z);
	SetWorldRotation(rot);
}

void Transform::SetLocalRotation(const D3DXVECTOR3 & rotation)
{
	D3DXQUATERNION quatRot;
	D3DXQuaternionRotationYawPitchRoll(&quatRot, rotation.y, rotation.x, rotation.z);

	D3DXMATRIX matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quatRot);

	this->rotation = rotation;

	axis[0] = D3DXVECTOR3(matRotate._11, matRotate._12, matRotate._13);
	axis[1] = D3DXVECTOR3(matRotate._21, matRotate._22, matRotate._23);
	axis[2] = D3DXVECTOR3(matRotate._31, matRotate._32, matRotate._33);

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::SetLocalRotation(const float & x, const float & y, const float & z)
{
	D3DXVECTOR3 rot(x, y, z);
	SetLocalRotation(rot);
}

void Transform::RotationLocal(const D3DXVECTOR3 & deltaRotation)
{
	D3DXQUATERNION quatRot;
	D3DXQuaternionRotationYawPitchRoll(&quatRot, deltaRotation.y, deltaRotation.x, deltaRotation.z);

	D3DXMATRIX matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quatRot);

	rotation = rotation += deltaRotation;

	for (int i = 0; i < 3; i++)
	{
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

D3DXMATRIX Transform::World() const
{
	return matWorld;
}

D3DXMATRIX Transform::Local() const
{
	return matLocal;
}

D3DXVECTOR3 Transform::GetWorldPosition() const
{
	D3DXVECTOR3 position = this->position;

	if (pParent)
		D3DXVec3TransformCoord(&position, &position, &pParent->World());

	return position;
}

D3DXVECTOR3 Transform::GetLocalPosition() const
{
	return position;
}

D3DXVECTOR3 Transform::GetScale() const
{
	return scale;
}

void Transform::GetAxis(D3DXVECTOR3 * pArr) const
{
	pArr[0] = axis[0];
	pArr[1] = axis[1];
	pArr[2] = axis[2];
}

D3DXVECTOR3 Transform::GetAxis(const UINT& i) const
{
	if (i > 2) return D3DXVECTOR3(0, 0, 0);

	return axis[i];
}

D3DXVECTOR3 Transform::GetAngle() const
{
	return rotation;
}

void Transform::UpdateTransform()
{
	D3DXMatrixIdentity(&matLocal);
	D3DXMatrixIdentity(&matWorld);

	if (rotation.x >= 360.0f) rotation.x -= 360.0f;
	else if (rotation.x <= -360.0f) rotation.x += 360.0f;
	if (rotation.y >= 360.0f) rotation.y -= 360.0f;
	else if (rotation.y <= -360.0f) rotation.y += 360.0f;
	if (rotation.z >= 360.0f) rotation.z -= 360.0f;
	else if (rotation.z <= -360.0f) rotation.z += 360.0f;

#if false
	D3DXMATRIX matScale, matRotate, matPosition;
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRotate);
	D3DXMatrixIdentity(&matPosition);

	D3DXMatrixScaling(&matScale, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&matPosition, position.x, position.y, position.z);
	D3DXMatrixRotationYawPitchRoll(&matRotate, rotation.y, rotation.x, rotation.z);

	matLocal = matScale * matRotate * matPosition;
#else
	D3DXVECTOR3 scaledAxis[3];
	for (UINT i = 0; i < 3; i++)
		scaledAxis[i] = axis[i] * scale[i];

	matLocal._11 = scaledAxis[0].x;
	matLocal._12 = scaledAxis[0].y;
	matLocal._13 = scaledAxis[0].z;
	matLocal._21 = scaledAxis[1].x;
	matLocal._22 = scaledAxis[1].y;
	matLocal._23 = scaledAxis[1].z;
	matLocal._31 = scaledAxis[2].x;
	matLocal._32 = scaledAxis[2].y;
	matLocal._33 = scaledAxis[2].z;
	matLocal._41 = position.x;
	matLocal._42 = position.y;
	matLocal._43 = position.z;
#endif

	if (this->pParent)
	{
		this->matWorld = matLocal * this->pParent->World();
	}
	else
	{
		this->matWorld = matLocal;
	}

	Transform* _pChild = this->pFirstChild;
	while (_pChild != NULL)
	{
		_pChild->UpdateTransform();
		_pChild = _pChild->pNextSibling;
	}
}
