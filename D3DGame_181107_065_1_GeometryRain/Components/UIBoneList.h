#pragma once
#include "../Components/ComponentUI.h"

class UIBoneList : public ComponentUI
{
public:
	UIBoneList();
	~UIBoneList();

	void Update() {};
	void Render();

	void ChangeTarget(class GameModel* target);
	int GetSelectedTargetBone() { return selectedTargetBone; }
	void SetSelectedTargetBone(int val) { selectedTargetBone = val; }

private:
	class GameModel* targetObject;

	int selectedTargetBone;

	char changeBoneName[1024];
	class ModelBone* changeTargetBone;

	int BoneTreeCreator(class GameModel* model, class ModelBone* bone);
};