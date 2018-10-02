#pragma once
#include "../Components/Component.h"

class UIBoneTransform : public Component
{
public:
	UIBoneTransform();
	~UIBoneTransform();

	void Update();
	void PostRender();

	void ChangeTarget(class ModelBone* targetBone, class GameModel* targetModel, class ModelClip* clip);
	void ChangeContainUIName(string containUIName);

private:
	string containUIName;
	class GameModel* targetModel;
	class ModelBone* targetBone;
	class ModelClip* targetClip;

	string strS, strR, strT;
	D3DXVECTOR3 vS, vR, vT;
	D3DXQUATERNION qR;
	D3DXMATRIX localMatrix;

	void ExportTransform();
	void CalcTranform();
};