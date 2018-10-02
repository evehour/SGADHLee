#pragma once
#include "../Components/Component.h"

class UIBoneList : public Component
{
public:
	UIBoneList();
	~UIBoneList();

	void Update();
	void PostRender();

	void ChangeTarget(class GameModel* target);
	void ChangeContainUIName(string containUIName);

private:
	string containUIName;
	class GameModel* targetObject;

	int BoneTreeCreator(class GameModel* model, ModelBone* bone);
};