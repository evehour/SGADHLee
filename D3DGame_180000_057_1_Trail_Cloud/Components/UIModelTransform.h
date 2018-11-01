#pragma once
#include "../Components/ComponentUI.h"

class UIModelTransform : public ComponentUI
{
public:
	UIModelTransform();
	~UIModelTransform();

	void Update();
	void Render();

	void ChangeTarget(class GameModel* target);

private:
	class GameModel* targetObject;

	D3DXVECTOR3 vS, vR, vT;
	D3DXQUATERNION qR;
};