#include "stdafx.h"
#include "GameRender.h"

#include "../Viewer/Camera.h"

SortInfo GameRender::sortArray[1024];

GameRender::GameRender()
	: orderWhenDraw(DrawOrderType::None)
	, visible(true)
	, position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
	, BoundObject(NULL)
{
	D3DXMatrixIdentity(&rootAxis);
	D3DXMatrixIdentity(&world);
}

GameRender::~GameRender()
{
	SAFE_DELETE(BoundObject);
}

void GameRender::Visible(bool val)
{
	visible = val;

	for (auto temp : Visibled)
		temp(val);
}

bool GameRender::Visible()
{
	return visible;
}

void GameRender::RootAxis(D3DXMATRIX & matrix)
{
	rootAxis = matrix;
}

void GameRender::RootAxis(D3DXVECTOR3 & rotation)
{
	D3DXMatrixRotationYawPitchRoll
	(
		&rootAxis
		, Math::ToRadian(rotation.y)
		, Math::ToRadian(rotation.x)
		, Math::ToRadian(rotation.z)
	);
}

D3DXMATRIX GameRender::RootAxis()
{
	return rootAxis;
}

D3DXMATRIX GameRender::World()
{
	return world;
}

void GameRender::Position(D3DXVECTOR3& vec)
{
	position = vec;

	UpdateWorld();
}

void GameRender::Position(float x, float y, float z)
{
	Position(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 GameRender::Position()
{
	return position;
}

void GameRender::Scale(D3DXVECTOR3 & vec)
{
	scale = vec;

	UpdateWorld();
}

void GameRender::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 GameRender::Scale()
{
	return scale;
}

void GameRender::Rotation(D3DXVECTOR3 & vec)
{
	rotation = vec;

	UpdateWorld();
}

void GameRender::Rotation(float x, float y, float z)
{
	Rotation(D3DXVECTOR3(x, y, z));
}

void GameRender::RotationDegree(D3DXVECTOR3 & vec)
{
	D3DXVECTOR3 temp;
	temp.x = Math::ToRadian(vec.x);
	temp.y = Math::ToRadian(vec.y);
	temp.z = Math::ToRadian(vec.z);

	Rotation(temp);
}

void GameRender::RotationDegree(float x, float y, float z)
{
	RotationDegree(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 GameRender::Rotation()
{
	return rotation;
}

D3DXVECTOR3 GameRender::RotationDegree()
{
	D3DXVECTOR3 temp;
	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	return temp;
}

D3DXVECTOR3 GameRender::Direction()
{
	return direction;
}

D3DXVECTOR3 GameRender::Up()
{
	return up;
}

D3DXVECTOR3 GameRender::Right()
{
	return right;
}

D3DXMATRIX GameRender::Transformed()
{
	return rootAxis * world;
}

void GameRender::Update()
{
}

void GameRender::OnRender()
{
	if (visible)
	{
		switch (orderWhenDraw)
		{
			case DrawOrderType::None:
				Render();

				for (NodeBase* child : childList)
				{
					GameRender* gameRender = dynamic_cast<GameRender *>(child);
					if (gameRender != nullptr)
						gameRender->OnRender();
				}
				break;
			case DrawOrderType::Ascending:
				break;
			case DrawOrderType::Descending:
				break;
		}
	}
}

void GameRender::Render()
{
}

void GameRender::DrawSort(bool ascending)
{
	int nodeCount = 0;
	UINT childCount = ChildCount();
	D3DXMATRIX View;
	Context::Get()->GetMainCamera()->Matrix(&View);
	
	for (UINT i = 0; i < childCount; ++i)
	{
		GameRender* child = dynamic_cast<GameRender*>(GetChild(i));

		if (child && child->Visible())
		{
			D3DXMATRIX modelView = child->Transformed() * View;
			sortArray[nodeCount].node = child;
			D3DXVECTOR3 _t = { modelView._41, modelView._42, modelView._43 };
			sortArray[nodeCount].depth = D3DXVec3Length(&_t);

			nodeCount++;
		}
	}

	QuickSort(sortArray, 0, nodeCount - 1, ascending);

	for (int i = 0; i < nodeCount; i++)
	{
		sortArray[i].node->OnRender();
		sortArray[i].Clear();
	}
}

void GameRender::QuickSort(SortInfo * data, int left, int right, bool ascending)
{
	int leftIndex = left;
	int rightIndex = right;
	float pivot = data[(left * right) / 2].depth;

	SortInfo temp;

	while (leftIndex <= rightIndex)
	{
		// search
		if (ascending)
		{
			for (; data[leftIndex].depth < pivot; leftIndex++);
			for (; data[rightIndex].depth > pivot; rightIndex--);
		}
		else
		{
			for (; data[leftIndex].depth > pivot; leftIndex++);
			for (; data[rightIndex].depth < pivot; rightIndex--);
		}

		// swap two values
		if (leftIndex <= rightIndex)
		{
			data[leftIndex].CopyTo(&temp);
			data[rightIndex].CopyTo(&data[leftIndex]);
			temp.CopyTo(&data[rightIndex]);

			leftIndex++;
			rightIndex--;
		}
	}

	// retry sorting
	if (rightIndex > left)
		QuickSort(data, left, rightIndex, ascending);

	// retry sorting
	if (leftIndex < right)
		QuickSort(data, leftIndex, right, ascending);
}

bool GameRender::Select(Ray * ray, float* distance)
{
	float dist = 0.0f;
	bool isIntersect = BoundObject->Intersect(ray, dist);

	if (distance != NULL) *distance = dist;

	return isIntersect;
}

void GameRender::UpdateWorld()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

#if true
	direction = D3DXVECTOR3(world._31, world._32, world._33);
	//direction *= -1.0f;//기본 모델의 방향에 따른 보정이 필요함.
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);
#else
	//기본 모델의 방향에 따른 보정이 필요하였음.
	D3DXVec3TransformNormal(&direction, &D3DXVECTOR3(0, 0, -1), &world);
	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &world);
	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &world);
#endif
}
