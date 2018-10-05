#pragma once
#include "../Components/ComponentUI.h"

class UIAnimationInformation : public ComponentUI
{
public:
	UIAnimationInformation();
	~UIAnimationInformation();

	void Update() override;
	void Render() override;

	void ChangeTarget(class ModelClip* targetClip, class GameModel* targetModel);

private:
	string clipName;
	UINT totalFrame;
	class GameModel* targetObject;
	class ModelClip* targetClip;

	D3DXVECTOR3 vS, vR, vT;
	D3DXQUATERNION qR;
};