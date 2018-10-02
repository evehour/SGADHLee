#pragma once
#include "../Components/Component.h"

class UIAnimationInformation : public Component
{
public:
	UIAnimationInformation();
	~UIAnimationInformation();

	void Update();
	void PostRender();

	void ChangeTarget(class ModelClip* targetClip, class GameModel* targetModel);
	void ChangeContainUIName(string containUIName);

private:
	string clipName;
	UINT totalFrame;
	string containUIName;
	class GameModel* targetObject;
	class ModelClip* targetClip;

	D3DXVECTOR3 vS, vR, vT;
	D3DXQUATERNION qR;
};