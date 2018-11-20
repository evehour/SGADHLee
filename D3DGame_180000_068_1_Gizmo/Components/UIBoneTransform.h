#pragma once
#include "../Components/ComponentUI.h"

class UIBoneTransform : public ComponentUI
{
public:
	UIBoneTransform();
	~UIBoneTransform();

	void Update();
	void Render();

	void ChangeTarget(class ModelBone* targetBone, class GameModel* targetModel, class ModelClip* clip);

private:
	class GameModel* targetModel;
	class ModelBone* targetBone;
	class ModelClip* targetClip;

	int radioMatrix;

	string strS, strR, strT;
	D3DXVECTOR3 vS, vR, vT;
	D3DXQUATERNION qR;
	D3DXMATRIX localMatrix;

	void ExportTransform();
	void CalcTranform();
};