#pragma once
#include "../Components/Component.h"

class UIModelTransform : public Component
{
public:
	UIModelTransform();
	~UIModelTransform();

	void Update();
	void PostRender();

	void ChangeTarget(class GameModel* target);
	void ChangeContainUIName(string containUIName);

private:
	string containUIName;
	class GameModel* targetObject;

	D3DXVECTOR3 vS, vR, vT;
	D3DXQUATERNION qR;
};