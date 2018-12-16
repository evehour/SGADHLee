#include "stdafx.h"
#include "AssetManager.h"

#include "../Model/ModelClip.h"
#include "../Objects/GameAnimModel.h"

AssetManager::AssetManager()
{
	models.clear();
	clips.clear();
	textures.clear();
}

AssetManager::~AssetManager()
{
	for (modelIter it = models.begin(); it != models.end();)
	{
		SAFE_DELETE(it->second);
		it = models.erase(it);
	}

	for (clipIter it = clips.begin(); it != clips.end();)
	{
		SAFE_DELETE(it->second);
		it = clips.erase(it);
	}

	for (textureIter it = textures.begin(); it != textures.end();)
	{
		SAFE_DELETE(it->second);
		it = textures.erase(it);
	}
}

void AssetManager::AddModel(wstring name, GameModel * model)
{
	if (models.count(name) > 0) return;

	models.insert(pair<wstring, GameModel*>(name, model));
}

void AssetManager::AddClip(wstring name, ModelClip * clip)
{
	if (clips.count(name) > 0) return;

	clips.insert(pair<wstring, ModelClip*>(name, clip));
}

void AssetManager::AddTexture(wstring name, Texture * texture)
{
	if (textures.count(name) > 0) return;

	textures.insert(pair<wstring, Texture*>(name, texture));
}

void AssetManager::RemModel(GameModel * model)
{
	wstring matName, meshName;
	auto iter = models.begin();

	matName = model->GetMaterialFileName();
	meshName = model->GetMeshFileName();

	while (iter != models.end())
	{
		if (
			iter->second->GetMaterialFileName() == matName &&
			iter->second->GetMeshFileName() == meshName
			)
		{
			SAFE_DELETE(iter->second);
			models.erase(iter);
			break;
		}
		else
			++iter;
	}
}

void AssetManager::RemModel(wstring name)
{
	if (models.count(name) < 1) return;

	SAFE_DELETE(models[name]);
	models.erase(name);
}

void AssetManager::RemClip(ModelClip * clip)
{
	wstring fileName = clip->GetFileName();
	clipIter iter = clips.begin();

	while (iter != clips.end())
	{
		if (iter->second->GetFileName() == fileName)
		{
			SAFE_DELETE(iter->second);
			clips.erase(iter);
			break;
		}
		else
			++iter;
	}
}

void AssetManager::RemClip(wstring name)
{
	if (clips.count(name) < 1) return;

	SAFE_DELETE(clips[name]);
	clips.erase(name);
}

void AssetManager::RemTexture(Texture * texture)
{
	wstring fileName = texture->GetFile();
	textureIter iter = textures.begin();

	while (iter != textures.end())
	{
		if (iter->second->GetFile() == fileName)
		{
			SAFE_DELETE(iter->second);
			textures.erase(iter);
			break;
		}
		else
			++iter;
	}
}

void AssetManager::RemTexture(wstring name)
{
	if (textures.count(name) < 1) return;

	SAFE_DELETE(textures[name]);
	textures.erase(name);
}

ModelClip * AssetManager::GetClip(const wstring name)
{
	if(clips.count(name) < 1)
		return nullptr;

	return clips[name];
}

Texture * AssetManager::GetTexture(const wstring name)
{
	if (textures.count(name) < 1)
		return nullptr;

	return textures[name];
}

void AssetManager::GetModelList(vector<wstring> & list)
{
	modelIter iter = models.begin();

	while (iter != models.end())
	{
		list.push_back(iter->first);
		++iter;
	}
}

void AssetManager::GetClipList(vector<wstring> & list)
{
	clipIter iter = clips.begin();

	while (iter != clips.end())
	{
		list.push_back(iter->first);
		++iter;
	}
}

void AssetManager::GetTextureList(vector<wstring> & list)
{
	textureIter iter = textures.begin();

	while (iter != textures.end())
	{
		list.push_back(iter->first);
		++iter;
	}
}

void AssetManager::CopyFromClips(const wstring name, OUT ModelClip ** clip)
{
	if (clips.count(name) < 1)
		return;

	*clip = new ModelClip(clips[name]);
}

void AssetManager::CopyFromTextures(const wstring name, OUT Texture ** texture)
{
	if (textures.count(name) < 1)
		return;

	*texture = new Texture(textures[name]->GetFile());
}

GameModel * AssetManager::GetModel(const wstring name)
{
	if (models.count(name) < 1)
		return nullptr;

	return models[name];
}

void AssetManager::CopyFromModels(const wstring name, OUT GameModel ** model)
{
	if (models.count(name) < 1)
		return;

	GameAnimModel* _animModel = NULL;
	_animModel = dynamic_cast<GameAnimModel*>(models[name]);

	if (_animModel == NULL)
		*model = new GameModel(models[name]);
	else
		*model = new GameAnimModel(_animModel);
}
