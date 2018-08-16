#pragma once
#include "GameModel.h"

class MeshObject : public GameModel
{
public:
	MeshObject(
		wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile,
		D3DXCOLOR diffuseColor = D3DXCOLOR(1,0,0,1));
	~MeshObject();

	void Update() override;
	void Render() override;


	// HW
public:
	enum Bound_Type
	{
		BOUND_TYPE_SPHERE = 0,
		BOUND_TYPE_BOX,
		BOUND_TYPE_MAX
	};
	void Center(D3DXVECTOR3& val) { val = center; }
	void BoundSize(D3DXVECTOR3& val) { val = boundSize; }
	void Radius(float& val) { val = radius; }

	void BoundType(Bound_Type& val) { boundType = val; }
	int BoundType() { return (int)boundType; }

	//0,2,4,6 - bottom
	void GetBoundSpace(std::vector<D3DXVECTOR3>& boundBox) { boundBox.assign(boundSpace.begin(), boundSpace.end()); }
	void GetBoundSpaceTries(std::vector<D3DXVECTOR3>& boundBoxTries) { boundBoxTries.assign(this->boundSpaceTries.begin(), this->boundSpaceTries.end()); }
	void GetAAABB(std::vector<D3DXVECTOR3>& aabbBox);

	D3DXVECTOR3 GetMinVertice() { return vecMin; }
	D3DXVECTOR3 GetMaxVertice() { return vecMax; }

	bool IsPick(D3DXVECTOR3& origin, D3DXVECTOR3& direction);

protected:
	class LineMake* box;
	class LineMake* boxAABB;
	std::vector<D3DXVECTOR3> boundSpace;
	std::vector<D3DXVECTOR3> boundSpaceTries;
	Bound_Type boundType;
	D3DXVECTOR3 center;
	D3DXVECTOR3 boundSize;
	D3DXVECTOR3 vecMin, vecMax;
	float radius;

	void SetBoundSpace();
};