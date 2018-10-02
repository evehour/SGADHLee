#include "stdafx.h"
#include "UIAssetManager.h"

#include "../Model/ModelClip.h"
#include "../Objects/GameAnimModel.h"

UIAssetManager::UIAssetManager()
{
	models.clear();
	clips.clear();
}

UIAssetManager::~UIAssetManager()
{
}

void UIAssetManager::AddModel(GameAnimModel * model)
{
	auto it = models.begin();
	while (it != models.end())
	{
		if ((*it) == model)
			return;

		it++;
	}

	models.push_back(model);
}

void UIAssetManager::AddClip(ModelClip * clip)
{
	auto it = clips.begin();
	while (it != clips.end())
	{
		if ((*it) == clip)
			return;

		it++;
	}

	clips.push_back(clip);
}

ModelClip * UIAssetManager::GetClip(const UINT & idx)
{
	if (idx >= clips.size())
		return nullptr;

	return clips[idx];
}

void UIAssetManager::CopyClip(const UINT & idx, OUT ModelClip * clip)
{
	if (idx >= clips.size())
		return;

	clip = new ModelClip(clips[idx]);
}

GameAnimModel * UIAssetManager::GetModel(const UINT & idx)
{
	return nullptr;
}

void UIAssetManager::CopoyModel(const UINT & idx, OUT GameAnimModel * model)
{
	if (idx >= models.size())
		return;

	model = new GameAnimModel(models[idx]);
}
