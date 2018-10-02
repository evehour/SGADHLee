#pragma once
#include "../Components/Component.h"

class UIMaterial : public Component
{
public:
	UIMaterial();
	~UIMaterial();

	void Update();
	void PostRender();

	void ChangeTarget(class GameModel* target);
	void ChangeContainUIName(string containUIName);

public:
	enum class TextureMap
	{
		Diffuse = 0,
		Normal,
		Specular,
	};

private:
	string containUIName;
	class GameModel* targetObject;

	UINT materialCount;
};