#include "stdafx.h"
#include "CapsuleCollider.h"
#include "../Physics/Collision.h"

CapsuleCollider::CapsuleCollider()
	: height(1.0f), radius(0.5f), band(60)
{
	CreateData();
	CreateBuffer();
	objType = 2;
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::Update()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, 1.0f, 1.0f, 1.0f);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	matLocal = S * R * T;

	if (matParent)
		matFinal = matLocal * (*matParent);
	else
		matFinal = matLocal;

	worldBuffer->SetMatrix(matFinal);
}

void CapsuleCollider::CreateData()
{
	vector<VertexColor> vCapsuleVertices;
	vector<UINT> vCapsuleIndices;

	float theta = Math::PI / (float)band;

	// Ã¹¹øÂ°
	{
		//Up
		for (int i = 0; i <= band; i++)
		{
			VertexColor v;
			float angle = (float)i * theta;

			v.Position = D3DXVECTOR3(
				cosf(angle) * radius, (height / 2.0f) + sinf(angle) * radius, 0);

			vCapsuleVertices.push_back(v);
		}

		//Down
		for (int i = 0; i <= band; i++)
		{
			VertexColor v;
			float angle = (float)i * theta + Math::PI;

			v.Position = D3DXVECTOR3(
				cosf(angle) * radius, -(height / 2.0f) + sinf(angle) * radius, 0);

			vCapsuleVertices.push_back(v);
		}

		for (UINT i = 0; i < vCapsuleVertices.size() - 1; i++)
		{
			vCapsuleIndices.push_back(i);
			vCapsuleIndices.push_back(i + 1);
		}

		vCapsuleIndices.push_back(vCapsuleIndices[vCapsuleIndices.size() - 1]);
		vCapsuleIndices.push_back(0);
	}


	{
		theta = Math::PI / 4.0f;
		D3DXMATRIX _matWorld;
		int verticesSize = vCapsuleVertices.size();
		int indicesSize = vCapsuleIndices.size();
		for (UINT i = 1; i < 4; i++)
		{
			D3DXMatrixRotationY(&_matWorld, (theta * (float)i));
			for (int j = 0; j < verticesSize; j++)
			{
				VertexColor v;
				D3DXVec3TransformCoord(&v.Position, &vCapsuleVertices[j].Position, &_matWorld);
				vCapsuleVertices.push_back(v);
			}

			for (int j = 0; j < verticesSize - 1; j++)
			{
				vCapsuleIndices.push_back((verticesSize * i) + j);
				vCapsuleIndices.push_back((verticesSize * i) + j + 1);
			}

			vCapsuleIndices.push_back(vCapsuleIndices[vCapsuleIndices.size() - 1]);
			vCapsuleIndices.push_back(vCapsuleIndices[(indicesSize * i)]);
		}
	}

	{
		theta = (Math::PI * 2.0f) / (float)band;

		int verticesSize = vCapsuleVertices.size();
		int indicesSize = vCapsuleIndices.size();

		// top circle
		for (int i = 0; i <= band; i++)
		{
			VertexColor v;
			float angle = (float)i * theta;
			v.Position = D3DXVECTOR3(cosf(angle) * radius, (height / 2.0f), sinf(angle) * radius);
			vCapsuleVertices.push_back(v);
		}

		for (int j = 0; j < band; j++)
		{
			vCapsuleIndices.push_back((verticesSize)+j);
			vCapsuleIndices.push_back((verticesSize)+j + 1);
		}

		//center circle
		verticesSize = vCapsuleVertices.size();
		indicesSize = vCapsuleIndices.size();
		for (int i = 0; i <= band; i++)
		{
			VertexColor v;
			float angle = (float)i * theta;
			v.Position = D3DXVECTOR3(cosf(angle) * radius, 0, sinf(angle) * radius);
			vCapsuleVertices.push_back(v);
		}

		for (int j = 0; j < band; j++)
		{
			vCapsuleIndices.push_back((verticesSize)+j);
			vCapsuleIndices.push_back((verticesSize)+j + 1);
		}

		//center circle
		verticesSize = vCapsuleVertices.size();
		indicesSize = vCapsuleIndices.size();
		for (int i = 0; i <= band; i++)
		{
			VertexColor v;
			float angle = (float)i * theta;
			v.Position = D3DXVECTOR3(cosf(angle) * radius, -(height / 2.0f), sinf(angle) * radius);
			vCapsuleVertices.push_back(v);
		}

		for (int j = 0; j < band; j++)
		{
			vCapsuleIndices.push_back((verticesSize)+j);
			vCapsuleIndices.push_back((verticesSize)+j + 1);
		}
	}

	vertexCount = vCapsuleVertices.size();
	indexCount = vCapsuleIndices.size();

	vertexData = new VertexColor[vertexCount];
	indexData = new UINT[indexCount];

	memcpy_s(vertexData, sizeof(VertexColor) * vertexCount, &vCapsuleVertices[0], sizeof(VertexColor) * vertexCount);
	memcpy_s(indexData, sizeof(UINT) * indexCount, &vCapsuleIndices[0], sizeof(UINT) * indexCount);

}

void CapsuleCollider::UpdateData()
{
	float theta = Math::PI / (float)band;
	float angle = 0.0f;
	int band = 60;
	int verticesSize = 0;

	// Ä¸½¶ ¸ð¾ç
	{
		//Up
		for (int i = 0; i <= band; i++)
		{
			angle = (float)i * theta;

			vertexData[i].Position = D3DXVECTOR3(
				cosf(angle) * radius, (height / 2.0f) + sinf(angle) * radius, 0);
		}

		//Down
		for (int i = 0; i <= band; i++)
		{
			angle = (float)i * theta + Math::PI;

			vertexData[(band + 1) + i].Position = D3DXVECTOR3(
				cosf(angle) * radius, -(height / 2.0f) + sinf(angle) * radius, 0);
		}
	}

	// + 4°³
	{
		theta = Math::PI / 4.0f;
		D3DXMATRIX _matWorld;
		verticesSize = (band + 1) * 2;
		for (UINT i = 1; i < 4; i++)
		{
			D3DXMatrixRotationY(&_matWorld, (theta * (float)i));
			for (int j = 0; j < verticesSize; j++)
			{
				D3DXVec3TransformCoord(&vertexData[(verticesSize) * i + j].Position, &vertexData[j].Position, &_matWorld);
			}
		}
	}

	{
		theta = (Math::PI * 2.0f) / (float)band;
		verticesSize = ((band + 1) * 2) * 4;

		// top circle
		for (int i = 0; i <= band; i++)
		{
			angle = (float)i * theta;
			vertexData[(verticesSize) + i].Position = D3DXVECTOR3(cosf(angle) * radius, (height / 2.0f), sinf(angle) * radius);
		}

		//center circle
		verticesSize = (((band + 1) * 2) * 4) + (band + 1);
		for (int i = 0; i <= band; i++)
		{
			angle = (float)i * theta;
			vertexData[(verticesSize) + i].Position = D3DXVECTOR3(cosf(angle) * radius, 0, sinf(angle) * radius);
		}

		//center circle
		verticesSize = (((band + 1) * 2) * 4) + ((band + 1) * 2);
		for (int i = 0; i <= band; i++)
		{
			angle = (float)i * theta;
			vertexData[(verticesSize) + i].Position = D3DXVECTOR3(cosf(angle) * radius, -(height / 2.0f), sinf(angle) * radius);
		}
	}

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertexData, vertexCount * sizeof(VertexColor), 0);
}

void CapsuleCollider::Scale(D3DXVECTOR3 & val) {
	this->scale = val;
	this->height = scale.y;
	this->radius = scale.x / 2.0f;
	UpdateData();
}

bool CapsuleCollider::IsCollision(Collider * other)
{
	bool _bCollision = false;
	switch (other->GetObjType())
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		// Capsule
		CapsuleCollider* otherCapsule = reinterpret_cast<CapsuleCollider*>(other);
		_bCollision = IsCollisionOtherCapsule(otherCapsule);
		break;
	}

	return _bCollision;
}

bool CapsuleCollider::IsCollisionOtherCapsule(CapsuleCollider * other)
{
	float s, t;
	D3DXVECTOR3 c1, c2;

	D3DXVECTOR3 a1, a2, b1, b2;
	a1 = D3DXVECTOR3(0,  (this->height / 2.0f), 0);
	a2 = D3DXVECTOR3(0, -(this->height / 2.0f), 0);
	b1 = D3DXVECTOR3(0,  (other->GetHeight() / 2.0f), 0);
	b2 = D3DXVECTOR3(0, -(other->GetHeight() / 2.0f), 0);

	D3DXVECTOR3 _S1, _S2, _T1, _T2;
	D3DXQUATERNION _R1, _R2;
	D3DXMatrixDecompose(&_S1, &_R1, &_T1, &GetMatrix());
	D3DXMatrixDecompose(&_S2, &_R2, &_T2, &other->GetMatrix());

	D3DXVec3TransformCoord(&a1, &a1, &GetMatrix());
	D3DXVec3TransformCoord(&a2, &a2, &GetMatrix());
	D3DXVec3TransformCoord(&b1, &b1, &other->GetMatrix());
	D3DXVec3TransformCoord(&b2, &b2, &other->GetMatrix());
	float dist2 = Collision::ClosestPtSegmentSegment(a1, a2, b1, b2, s, t, c1, c2);
	float radius = (this->radius * _S1.x) + (other->GetRadius() * _S2.x);

	return dist2 <= (radius * radius);
}

bool CapsuleCollider::IsCollisionRay(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{
	D3DXVECTOR3 AB;
	return false;
}
