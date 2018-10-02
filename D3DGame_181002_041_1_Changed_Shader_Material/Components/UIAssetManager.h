#pragma once
#include "Component.h"

class UIAssetManager : public Component
{
public:
	UIAssetManager();
	~UIAssetManager();

public:
	void AddModel(class GameAnimModel* model);
	void AddClip(class ModelClip* clip);

	class ModelClip* GetClip(const UINT& idx);
	void CopyClip(const UINT& idx, OUT class ModelClip* clip);

	class GameAnimModel* GetModel(const UINT& idx);
	void CopoyModel(const UINT& idx, OUT class GameAnimModel* model);

private:
	vector<class GameAnimModel*> models;
	vector<class ModelClip*> clips;
};