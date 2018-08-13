#pragma once
#include "GameRender.h"

class GameModel : public GameRender
{
public:
	enum Bound_Type
	{
		BOUND_TYPE_SPHERE = 0,
		BOUND_TYPE_BOX,
		BOUND_TYPE_MAX
	};

	GameModel
	(
		wstring matFolder, wstring matFile
		, wstring meshFolder, wstring meshFile
	);
	virtual ~GameModel();

	void Velocity(D3DXVECTOR3& vec);
	D3DXVECTOR3 Velocity();

	virtual void Update();
	virtual void Render();

	void Rotate(D3DXVECTOR2 amount);

protected:
	void CalcPosition();

protected:
	Model* model;
	Shader* shader;

	D3DXVECTOR3 velocity;

	D3DXMATRIX matRotation;
	vector<D3DXMATRIX> boneTransforms;

private:
	class RenderBuffer : public ShaderBuffer
	{
	public:
		RenderBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Index = 0;
		}

		struct Struct
		{
			int Index;

			float Padding[3];
		} Data;
	};

	RenderBuffer* renderBuffer;

public:
	// HW
	void Center(D3DXVECTOR3& val) { val = center; }
	void BoundSize(D3DXVECTOR3& val) { val = boundSize; }
	void Radius(float& val) { val = radius; }

	void BoundType(Bound_Type val) { boundType = val; }
	int BoundType() { return (int)boundType; }

	void GetAAABB(std::vector<D3DXVECTOR3>& aabbBox);

	D3DXVECTOR3 GetMinVertice() { return vecMin; }
	D3DXVECTOR3 GetMaxVertice() { return vecMax; }

protected:
	// HW
	std::vector<D3DXVECTOR3> boundSpace;
	Bound_Type boundType;
	D3DXVECTOR3 center;
	D3DXVECTOR3 boundSize;
	D3DXVECTOR3 vecMin, vecMax;
	float radius;

	class LineMake *gizmo, *gizmoAABB;

	void SetBoundSpace();
};