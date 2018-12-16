#pragma once
#include "./Interfaces/IBoundable.h"
#include "./Components/GameNode.h"

struct SortInfo
{
	class GameRender* node;
	float depth;

	SortInfo()
		: node(NULL), depth(0.0f) {}

	void Clear()
	{
		this->node = NULL;
		this->depth = 0.0f;
	}

	void CopyTo(SortInfo* target)
	{
		target->node = this->node;
		target->depth = this->depth;
	}
};

class GameRender : public GameNode
{
public:
	enum class DrawOrderType
	{
		None = 0,
		Ascending,
		Descending,
	};

public:
	GameRender();
	virtual ~GameRender();

	void Visible(bool val);
	bool Visible();
	vector<function<void(bool)>> Visibled;

	DrawOrderType OrderWhenDraw() { return orderWhenDraw; }
	void OrderWhenDraw(DrawOrderType val) { orderWhenDraw = val; }

	void RootAxis(D3DXMATRIX& matrix);
	void RootAxis(D3DXVECTOR3& rotation);
	D3DXMATRIX RootAxis();

	D3DXMATRIX World();

	void Position(D3DXVECTOR3& vec);
	void Position(float x, float y, float z = 0.0f);
	D3DXVECTOR3 Position();

	void Scale(D3DXVECTOR3& vec);
	void Scale(float x, float y, float z = 1.0f);
	D3DXVECTOR3 Scale();

	void Rotation(D3DXVECTOR3& vec);
	void Rotation(float x, float y, float z);
	void RotationDegree(D3DXVECTOR3& vec);
	void RotationDegree(float x, float y, float z);
	D3DXVECTOR3 Rotation();
	D3DXVECTOR3 RotationDegree();

	D3DXVECTOR3 Direction();
	D3DXVECTOR3 Up();
	D3DXVECTOR3 Right();

	D3DXMATRIX Transformed();

	virtual void Update();
	virtual void OnRender();
	virtual void Render();

	void DrawSort(bool ascending);
	void QuickSort(SortInfo* data, int left, int right, bool ascending);

	virtual bool Select(struct Ray* ray, float* distance);

protected:
	virtual void UpdateWorld();

protected:
	DrawOrderType orderWhenDraw;
	bool visible;

	D3DXMATRIX rootAxis;

	class IBoundable* BoundObject;

	D3DXMATRIX world;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;

	static SortInfo sortArray[1024];
};