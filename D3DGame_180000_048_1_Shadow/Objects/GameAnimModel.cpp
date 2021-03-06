#include "stdafx.h"
#include "GameAnimModel.h"
#include "../Model/ModelBone.h"
#include "../Model/ModelClip.h"
#include "../Model/ModelTweener.h"

GameAnimModel::GameAnimModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: GameModel(matFolder, matFile, meshFolder, meshFile)
	, playTime(0.0f), isAnimMatrixUpdate(true)
{
	model->Buffer()->UseBlend(true);
	tweener = new ModelTweener();
	shader = new Shader(Shaders + L"035_Animation_HW.hlsl");
	SetShader(shader);
}

GameAnimModel::GameAnimModel(const GameAnimModel * T)
	: GameModel(T->matFolder, T->matFile, T->meshFolder, T->meshFile)
{
	this->playTime = T->playTime;
	this->isAnimMatrixUpdate = T->isAnimMatrixUpdate;
	this->name = T->name;
	this->currentIdx = T->currentIdx;

	tweener = new ModelTweener();

	auto itClip = T->clips.begin();
	while (itClip != T->clips.end())
	{
		ModelClip* c = new ModelClip((*itClip));
		AddClip(c);
	}

	shader = new Shader(Shaders + L"035_Animation_HW.hlsl");
	SetShader(shader);
}

GameAnimModel::~GameAnimModel()
{
	SAFE_DELETE(shader);

	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);

	SAFE_DELETE(tweener);
}

void GameAnimModel::Update(bool bDeltaMode)
{
	if (clips.size() < 1)
	{
		__super::Update();
		return;
	}

	if (isAnimMatrixUpdate)
	{
		CalcPosition();
		for (UINT i = 0; i < model->BoneCount(); i++)
		{
			ModelBone* bone = model->BoneByIndex(i);

			tweener->UpdateBlending(bone, (bDeltaMode ? Time::Delta() : playTime), bDeltaMode);
		}
		tweener->TriggerCheck();
	}
	__super::Update();
}

void GameAnimModel::Update()
{
	if (clips.size() < 1)
	{
		__super::Update();
		return;
	}

	if (isAnimMatrixUpdate)
	{
		CalcPosition();
		for (UINT i = 0; i < model->BoneCount(); i++)
		{
			ModelBone* bone = model->BoneByIndex(i);

			tweener->UpdateBlending(bone, Time::Delta(), true);
		}
		tweener->TriggerCheck();
	}
	__super::Update();
}

UINT GameAnimModel::AddClip(wstring file, float startTime)
{
	ModelClip* clip = new ModelClip(file);

	return AddClip(clip);
}

UINT GameAnimModel::AddClip(ModelClip * clip)
{
	UINT i = ContainClip(clip);
	
	if (i > -1)
	{
		return i;
	}
	else
	{
		clips.push_back(clip);
	}

	return clips.size() - 1;
}

UINT GameAnimModel::DelClip(ModelClip * clip)
{
	vector<ModelClip *>::iterator it = clips.begin();
	while (it != clips.end())
	{
		if ((*it)->GetFileName() == clip->GetFileName())
		{
			clips.erase(it);
			break;
		}
		it++;
	}

	return clips.size() - 1;
}

UINT GameAnimModel::DelClip(wstring name)
{
	vector<ModelClip *>::iterator it = clips.begin();
	while (it != clips.end())
	{
		if ((*it)->GetName() == name)
		{
			clips.erase(it);
			break;
		}
		it++;
	}

	return clips.size() - 1;
}

void GameAnimModel::RemoveAllClip(bool inCase)
{
	vector<class ModelClip *>::iterator it = clips.begin();
	while (it != clips.end())
	{
		if (inCase)
			SAFE_DELETE((*it));

		it = clips.erase(it);
	}
}

int GameAnimModel::ContainClip(ModelClip * clip)
{
	return ContainClip(clip->GetName());
}

int GameAnimModel::ContainClip(wstring name)
{
	for (UINT i = 0; i < clips.size(); i++)
	{
		if (clips[i]->GetName() == name)
			return (int)i;
	}
	return -1;
}

void GameAnimModel::LockRoot(UINT index, bool val) { clips[index]->LockRoot(val); }
void GameAnimModel::Repeat(UINT index, bool val) { clips[index]->Repeat(val); }
void GameAnimModel::Speed(UINT index, float val) { clips[index]->Speed(val); }

void GameAnimModel::Play(UINT index, bool bRepeat, float blendTime, float speed, float startTime)
{
	currentIdx = index;
	tweener->Play(clips[index], bRepeat, blendTime, speed, startTime);
}

void GameAnimModel::Play(ModelClip * clip, bool bRepeat, float blendTime, float speed, float startTime)
{
	int idx = ContainClip(clip);
	if (idx < 0)
	{
		return;
	}
	
	tweener->Play(clips[idx], bRepeat, blendTime, speed, startTime);
}

float GameAnimModel::GetClipDuration(UINT i)
{
	if (i >= clips.size()) return 0.0f;

	return clips[i]->GetDuration();
}

float GameAnimModel::GetClipDuration(ModelClip * clip)
{
	vector<class ModelClip *>::iterator it = clips.begin();

	while (it != clips.end())
	{
		if ((*it) == clip)
		{
			return (*it)->GetDuration();
		}

		it++;
	}

	return 0.0f;
}

void GameAnimModel::AddCurrentMotion(UINT index, float time)
{
	if (clips.size() <= index) return;

	for (ModelBone* bone : model->Bones())
	{
		ModelKeyframe::Transform tf;
		D3DXMATRIX matrix, parent;
		D3DXMatrixInverse(&matrix, NULL, &boneTransforms[bone->Index()]);
		matrix = matrix * bone->Local();

		D3DXMatrixDecompose(&tf.S, &tf.R, &tf.T, &matrix);
		tf.Time = time;

		clips[index]->AddMotion(bone, tf);
	}

	ModelKeyframe::Transform dummy;
	clips[index]->AddMotion(NULL, dummy, true);
}
