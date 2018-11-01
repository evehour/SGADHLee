#pragma once
#include "./Component.h"

class ComponentUI : public Component
{
public:
	ComponentUI();
	virtual ~ComponentUI();

	virtual void Update() = 0;
	virtual void Render() = 0;

	void ChangeContainUIName(string containUIName) { this->containUIName = containUIName; };

public:
	enum class UIType
	{
		UIBoneList = 0,
		UIBoneTransform,
		UIMaterial,
		UIModelTransform,
		UIAnimationInformation,
	};

	UIType GetUIType() { return this->uiType; }

protected:
	UIType uiType;
	string containUIName;
};