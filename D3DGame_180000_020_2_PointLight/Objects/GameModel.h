#pragma once
#include "GameRender.h"

class GameModel : public GameRender
{
public:
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

	Model* GetModel() { return model; }

	void SetShader(Shader* shader);

	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(D3DXCOLOR& color);
	void SetDiffuseMap(wstring file);

	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetSpecular(D3DXCOLOR& color);
	void SetSpecularMap(wstring file);

	void SetNormalMap(wstring file);

	void SetShininess(float val);

protected:
	void CalcPosition();

protected:
	Model* model;
	Shader* shader;

	D3DXVECTOR3 velocity;

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

	void BoundType(Bound_Type& val) { boundType = val; }
	int BoundType() { return (int)boundType; }

	//0,2,4,6 - bottom
	void GetBoundSpace(std::vector<D3DXVECTOR3>& boundBox) { boundBox.assign(boundSpace.begin(), boundSpace.end()); }
	void GetBoundSpaceTries(std::vector<D3DXVECTOR3>& boundBoxTries) { boundBoxTries.assign(this->boundSpaceTries.begin(), this->boundSpaceTries.end()); }
	void GetAAABB(std::vector<D3DXVECTOR3>& aabbBox);

	D3DXVECTOR3 GetMinVertice() { return vecMin; }
	D3DXVECTOR3 GetMaxVertice() { return vecMax; }

	//bool IsPick(D3DXVECTOR3& origin, D3DXVECTOR3& direction, OUT D3DXVECTOR3& position);
	bool IsPickPart(D3DXVECTOR3& origin, D3DXVECTOR3& direction, OUT int& partIdx, OUT D3DXVECTOR3& position);
	void SetBoneMatrixByIdx(const int& boneIdx, const D3DXMATRIX& matrix);
	string GetBoneNameByIdx(const int& boneIdx);
	D3DXMATRIX GetBoneLocalMatrixByIdx(const int& boneIdx);


protected:
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
};