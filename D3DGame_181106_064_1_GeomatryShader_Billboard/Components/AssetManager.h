#pragma once
#include "Component.h"

class AssetManager : public Component
{
public:
	AssetManager();
	~AssetManager();

	void Update();
	void Render();

public:
	void AddModel(class GameAnimModel* model);
	void AddClip(class ModelClip* clip);

	void RemModel(class GameAnimModel* model);
	void RemModel(UINT idx);
	void RemClip(class ModelClip* clip);
	void RemClip(UINT idx);

	UINT GetModelCount() { return modelCount; }
	UINT GetClipCount() { return clipCount; }

	class GameAnimModel* GetModel(const UINT& idx);
	class ModelClip* GetClip(const UINT& idx);

	void CopyFromModels(const UINT& idx, OUT class GameAnimModel** model);
	void CopyFromClips(const UINT& idx, OUT class ModelClip** clip);

private:
	vector<class GameAnimModel*> models;
	vector<class ModelClip*> clips;

	UINT modelCount, clipCount;
};