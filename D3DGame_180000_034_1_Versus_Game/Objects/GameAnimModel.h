#pragma once
#include "GameModel.h"

class GameAnimModel : public GameModel
{
public:
	GameAnimModel
	(
		wstring matFolder, wstring matFile
		, wstring meshFolder, wstring meshFile
	);
	virtual ~GameAnimModel();

	virtual void Update(bool bDeltaMode = false);

	UINT AddClip(wstring file, float startTime = 0.0f);
	UINT AddClip(class ModelClip* clip);

	UINT DelClip(class ModelClip* clip);
	UINT DelClip(wstring name);

	void RemoveAllClip(bool inCase = true);

	int ContainClip(class ModelClip* clip);
	int ContainClip(wstring name);
	int GetClipCount() const { return clips.size(); }

	void LockRoot(UINT index, bool val);
	void Repeat(UINT index, bool val);
	void Speed(UINT index, float val);

	void Play
	(
		UINT index
		, bool bRepeat
		, float blendTime = 0.0f
		, float speed = 1.0f
		, float startTime = 0.0f
	);

	void Play
	(
		class ModelClip* clip
		, bool bRepeat
		, float blendTime = 0.0f
		, float speed = 1.0f
		, float startTime = 0.0f
	);

	void SetName(const wstring& name) { this->name = name; }
	wstring GetName() const { return this->name; }

	void SetTime(const float& val) { this->playTime = val; }
	float GetTime() { return this->playTime; }

	float GetClipDuration(UINT i);
	float GetClipDuration(class ModelClip * clip);

	bool GetAnimMatrixUpdate() { return isAnimMatrixUpdate; }
	void SetAnimMatrixUpdate(bool& val) { isAnimMatrixUpdate = val; }

	void AddCurrentMotion(UINT index, float time);

	UINT GetCurrentPlayIndex() { return currentIdx; }

private:
	wstring name;
	float playTime;
	bool isAnimMatrixUpdate;
	UINT currentIdx;

private:
	vector<class ModelClip *> clips;
	class ModelTweener* tweener;
};