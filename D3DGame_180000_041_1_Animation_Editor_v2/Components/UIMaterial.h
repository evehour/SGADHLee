#pragma once
#include "../Components/ComponentUI.h"

class UIMaterial : public ComponentUI
{
public:
	UIMaterial();
	~UIMaterial();

	void Update();
	void Render();

	void ChangeTarget(class GameModel* target);

public:
	enum class TextureMap
	{
		Diffuse = 0,
		Normal,
		Specular,
	};

private:
	class GameModel* targetObject;

	UINT materialCount;
};