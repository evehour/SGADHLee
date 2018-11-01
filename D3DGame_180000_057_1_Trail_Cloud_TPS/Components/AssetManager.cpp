#include "stdafx.h"
#include "AssetManager.h"

#include "../Model/ModelClip.h"
#include "../Objects/GameAnimModel.h"

AssetManager::AssetManager()
	: modelCount(0), clipCount(0)
{
	models.clear();
	clips.clear();
}

AssetManager::~AssetManager()
{
	for (UINT i = 0; i < models.size(); i++)
		SAFE_DELETE(models[i]);

	for (UINT i = 0; i < clips.size(); i++)
		SAFE_DELETE(clips[i]);
}

void AssetManager::Update()
{
}

void AssetManager::Render()
{
}

void AssetManager::AddModel(GameAnimModel * model)
{
	auto it = models.begin();
	while (it != models.end())
	{
		if ((*it) == model)
			return;

		it++;
	}

	models.push_back(model);
	modelCount++;
}

void AssetManager::AddClip(ModelClip * clip)
{
	auto it = clips.begin();
	while (it != clips.end())
	{
		if ((*it) == clip)
			return;

		it++;
	}

	clips.push_back(clip);
	clipCount++;
}

void AssetManager::RemModel(GameAnimModel * model)
{
	wstring matName, meshName;
	auto iter = models.begin();

	matName = model->GetMaterialFileName();
	meshName = model->GetMeshFileName();

	while (iter != models.end())
	{
		if (
			(*iter)->GetMaterialFileName() == matName &&
			(*iter)->GetMeshFileName() == meshName
			)
		{
			SAFE_DELETE((*iter));
			models.erase(iter);
			--modelCount;
			break;
		}

		++iter;
	}
}

void AssetManager::RemModel(UINT idx)
{
	if (idx >= modelCount) return;

	auto iter = models.begin() + idx;
	SAFE_DELETE((*iter));
	models.erase(iter);
	--modelCount;
}

void AssetManager::RemClip(ModelClip * clip)
{
	wstring fileName = clip->GetFileName();
	auto iter = clips.begin();

	while (iter != clips.end())
	{
		if ((*iter)->GetFileName() == fileName)
		{
			SAFE_DELETE((*iter));
			clips.erase(iter);
			clipCount--;
			break;
		}

		++iter;
	}
}

void AssetManager::RemClip(UINT idx)
{
	if (idx >= clipCount) return;

	auto iter = clips.begin() + idx;
	SAFE_DELETE((*iter));
	clips.erase(iter);
	--clipCount;
}

ModelClip * AssetManager::GetClip(const UINT & idx)
{
	if (idx >= clipCount)
		return nullptr;

	return clips[idx];
}

void AssetManager::CopyFromClips(const UINT & idx, OUT ModelClip ** clip)
{
	if (idx >= clipCount)
		return;

	*clip = new ModelClip(clips[idx]);
}

GameAnimModel * AssetManager::GetModel(const UINT & idx)
{
	if (idx >= modelCount)
		return nullptr;

	return models[idx];
}

void AssetManager::CopyFromModels(const UINT & idx, OUT GameAnimModel ** model)
{
	if (idx >= modelCount)
		return;

	*model = new GameAnimModel(models[idx]);
}
