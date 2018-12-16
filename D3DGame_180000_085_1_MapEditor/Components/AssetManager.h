#pragma once

class AssetManager
{
public:
	AssetManager();
	~AssetManager();

public:
	void AddModel(wstring name, class GameModel* model);
	void AddClip(wstring name, class ModelClip* clip);
	void AddTexture(wstring name, Texture* texture);

	void RemModel(class GameModel* model);
	void RemModel(wstring name);
	void RemClip(class ModelClip* clip);
	void RemClip(wstring name);
	void RemTexture(class Texture* texture);
	void RemTexture(wstring name);

	UINT GetModelCount() { return models.size(); }
	UINT GetClipCount() { return clips.size(); }
	UINT GetTextureCount() { return textures.size(); }

	class GameModel* GetModel(const wstring name);
	class ModelClip* GetClip(const wstring name);
	class Texture* GetTexture(const wstring name);

	void GetModelList(vector<wstring>& list);
	void GetClipList(vector<wstring>& list);
	void GetTextureList(vector<wstring>& list);

	void CopyFromModels(const wstring name, OUT class GameModel** model);
	void CopyFromClips(const wstring name, OUT class ModelClip** clip);
	void CopyFromTextures(const wstring name, OUT class Texture** texture);

private:
	typedef map<wstring, class GameModel*>::iterator modelIter;
	typedef map<wstring, class ModelClip*>::iterator clipIter;
	typedef map<wstring, class Texture*>::iterator textureIter;

	map<wstring, class GameModel*> models;
	map<wstring, class ModelClip*> clips;
	map<wstring, Texture*> textures;
};