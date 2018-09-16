#pragma once
#include "ModelKeyframe.h"

class ModelClip
{
public:
	ModelClip(wstring file);
	~ModelClip();

	void Reset();

	D3DXMATRIX GetKeyframeMatrix(class ModelBone* bone, float time, bool isDelta = true);
	D3DXMATRIX GetKeyframeOriginMatrix(class ModelBone* bone, float time, bool isDelta = true);
	void UpdateKeyframe(class ModelBone* bone, float time);

	void LockRoot(bool val) { bLockRoot = val; }
	void Repeat(bool val) { bRepeat = val; }
	void Speed(float val) { speed = val; }
	void StartTime(float val) { playTime = val; }

	void SetName(const wstring& name) { this->name = name; }
	wstring GetName() const { return this->name; }

	float GetDuration() const { return this->duration; }

	float GetPlayTime() const { return this->playTime; }
	void SetPlayTime(const float& val) { this->playTime = val; }

	void WriteClip(wstring file);
	void AddMotion(class ModelBone* bone, ModelKeyframe::Transform tf, bool incGlobalFrameCount = false);

	void GetKeyframeTransform(class ModelBone* bone, OUT vector<ModelKeyframe::Transform>& vec);
private:
	wstring name;

	float duration;
	float frameRate;
	UINT frameCount;

	bool bLockRoot;
	bool bRepeat;
	float speed;
	float playTime;

	//vector<class ModelKeyframe *> keyframes;
	typedef pair<wstring, ModelKeyframe *> Pair;
	unordered_map<wstring, ModelKeyframe *> keyframeMap;
};