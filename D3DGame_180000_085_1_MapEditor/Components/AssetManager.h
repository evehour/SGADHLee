#pragma once

class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	void Update();
	void Render();

public:
	void AddModel(class GameModel* model);
	void AddClip(class ModelClip* clip);

	void RemModel(class GameModel* model);
	void RemModel(UINT idx);
	void RemClip(class ModelClip* clip);
	void RemClip(UINT idx);

	UINT GetModelCount() { return modelCount; }
	UINT GetClipCount() { return clipCount; }

	class GameModel* GetModel(const UINT& idx);
	class ModelClip* GetClip(const UINT& idx);

	void CopyFromModels(const UINT& idx, OUT class GameModel** model);
	void CopyFromClips(const UINT& idx, OUT class ModelClip** clip);

private:
	vector<class GameModel*> models;
	vector<class ModelClip*> clips;

	UINT modelCount, clipCount;
};