#include "stdafx.h"
#include "Camera.h"

#include "Viewport.h"
#include "Perspective.h"

Camera::Camera()
	: position(0, 0, 0), rotation(0, 0)
	, forward(0, 0, 1), right(1, 0, 0), up(0, 1, 0)
{
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matView);

	Move();
	Rotation();
}

Camera::~Camera()
{
}

D3DXVECTOR3 Camera::Direction(const D3DXVECTOR3 & pos)
{
	D3DXVECTOR3 screenSize;
	screenSize.x = Context::Get()->GetViewport()->GetWidth();
	screenSize.y = Context::Get()->GetViewport()->GetHeight();

	D3DXVECTOR2 point; // 뷰포트 역변환 된 결과

					   //Inv Viewport
	{
		point.x = (2.0f * pos.x) / screenSize.x - 1.0f;
		point.y = ((2.0f * pos.y) / screenSize.y - 1.0f) * -1.0f;
	}

	//Inv Projection
	{
		D3DXMATRIX projection;
		Context::Get()->GetPerspective()->GetMatrix(&projection);

		point.x = point.x / projection._11;
		point.y = point.y / projection._22;
	}

	D3DXVECTOR3 dir(0, 0, 0);
	// Inv View
	{
		D3DXMATRIX invView;
		D3DXMatrixInverse(&invView, NULL, &matView);

		//dir.x = (point.x * invView._11) + (point.y * invView._21) + invView._31;
		//dir.y = (point.x * invView._12) + (point.y * invView._22) + invView._32;
		//dir.z = (point.x * invView._13) + (point.y * invView._23) + invView._33;
		// 위 식이 아래식으로 됨. 왜냐하면 트랜스폼노멀에서 z를 1로 줬을 때와 같기 때문.
		D3DXVec3TransformNormal(&dir, &D3DXVECTOR3(point.x, point.y, 1), &invView);
		D3DXVec3Normalize(&dir, &dir);
	}

	return dir;
}

void Camera::Move()
{
	View();
}

void Camera::Rotation()
{
	D3DXMATRIX x, y;
	D3DXMatrixRotationX(&x, rotation.x);
	D3DXMatrixRotationY(&y, rotation.y);

	matRotation = x * y;


	D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &matRotation);
	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &matRotation);
	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &matRotation);

	View();
}

void Camera::View()
{
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}
