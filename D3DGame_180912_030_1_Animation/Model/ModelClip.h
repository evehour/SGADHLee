#pragma once
#include "ModelKeyframe.h"

class ModelClip
{
public:
	ModelClip(wstring file, float startTime = 0.0f);
	~ModelClip();

	// 애니메이션과 본이 일치되지 않을 가능성이 훨씬 높기 때문에
	void GetKeyframeMatrix(
		class ModelBone* bone //해당 플레이가 될 본
		, float time);

	void LockRoot(bool val) { bLockRoot = val; }
	void Repeat(bool val) { bRepeat = val; }
	void Speed(float val) { speed = val; }

private:
	wstring name;

	float duration;
	float frameRate;
	UINT frameCount;

	bool bLockRoot;
	bool bRepeat;
	float speed; // 이 애니메이션 클립의 속도
	float  playTime; // ms 단위

	//vector<class ModelKeyframe *> keyframes;
	typedef pair<wstring, ModelKeyframe *> Pair;
	unordered_map<wstring, ModelKeyframe *> keyframeMap;
};