#pragma once
#include "GameRender.h"
#include "../Model/Model.h"
#include "../Model/ModelMesh.h"
#include "../Model/ModelMeshPart.h"

class GameModel : public GameRender
{
public:
	GameModel
	(
		wstring shaderFile,
		wstring matFolder, wstring matFile
		, wstring meshFolder, wstring meshFile
	);
	virtual ~GameModel();

	virtual std::thread* UpdateThread();
	virtual void Update();
	virtual void Update(bool bDeltaMode);
	virtual void Render();

	Model* GetModel() { return model; }

	void SetShader(wstring shaderFile);

	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(D3DXCOLOR& color);
	void SetDiffuseMap(wstring file);

	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetSpecular(D3DXCOLOR& color);
	void SetSpecularMap(wstring file);

	void SetNormalMap(wstring file);

	void SetDetailMap(wstring file);

	void SetShininess(float val);

	D3DXMATRIX* GetBoneTransforms() { return transforms; }
	void BoneIndex(UINT index) { boneIndex = index; }

protected:
	void UpdateWorld() override;
	
protected:
	Model* model;

	UINT boneIndex;

	D3DXMATRIX* transforms;
	thread updateThreader;


public:
	bool IsPick(D3DXVECTOR3 & origin, D3DXVECTOR3 & direction, OUT D3DXVECTOR3 & position);

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


	void SetBounder(Bound_Type val);
	void BoundType(Bound_Type& val) { boundType = val; }
	int BoundType() { return (int)boundType; }

	//0,2,4,6 - bottom
	void GetBoundSpace(std::vector<D3DXVECTOR3>& boundBox) { boundBox.assign(boundSpace.begin(), boundSpace.end()); }
	void GetBoundSpaceTries(std::vector<D3DXVECTOR3>& boundBoxTries) { boundBoxTries.assign(this->boundSpaceTries.begin(), this->boundSpaceTries.end()); }
	void GetAAABB(std::vector<D3DXVECTOR3>& aabbBox);

	D3DXVECTOR3 GetMinVertice() { return vecMin; }
	D3DXVECTOR3 GetMaxVertice() { return vecMax; }

	//bool IsPick(D3DXVECTOR3& origin, D3DXVECTOR3& direction, OUT D3DXVECTOR3& position);
	bool IsPickPart(D3DXVECTOR3& origin, D3DXVECTOR3& direction, OUT int& partIdx, OUT D3DXVECTOR3* position);
	void SetBoneMatrixByIdx(const int& boneIdx, const D3DXMATRIX& matrix);
	string GetBoneNameByIdx(const int& boneIdx);
	D3DXMATRIX GetBoneLocalMatrixByIdx(const int& boneIdx);

	void SettingDefaultPose();

protected:
	wstring matFolder, matFile, meshFolder, meshFile;
	class LineMake* box;
	class LineMake* boxAABB;
	std::vector<D3DXVECTOR3> boundSpace;
	std::vector<D3DXVECTOR3> boundSpaceTries;

	std::vector<pair<int, pair<class ModelBone *, D3DXMATRIX>>> boneList;

	Bound_Type boundType;
	D3DXVECTOR3 center;
	D3DXVECTOR3 boundSize;
	D3DXVECTOR3 vecMin, vecMax;
	float radius;

	void SetBoundSpace();
	void SetBoneList();

protected:
	vector<D3DXMATRIX> defaultBoneLocalMatrix;
	vector<D3DXMATRIX> defaultBoneGlobalMatrix;

public:
	wstring GetMaterialFileName() { return matFolder + matFile; }
	wstring GetMeshFileName() { return meshFolder + meshFile; }

	ModelBone* AddBone();
};