#pragma once
#include "ModelKeyframe.h"

class ModelClip
{
public:
	ModelClip(wstring file, float startTime = 0.0f);
	~ModelClip();

	// �ִϸ��̼ǰ� ���� ��ġ���� ���� ���ɼ��� �ξ� ���� ������
	void GetKeyframeMatrix(
		class ModelBone* bone //�ش� �÷��̰� �� ��
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
	float speed; // �� �ִϸ��̼� Ŭ���� �ӵ�
	float  playTime; // ms ����

	//vector<class ModelKeyframe *> keyframes;
	typedef pair<wstring, ModelKeyframe *> Pair;
	unordered_map<wstring, ModelKeyframe *> keyframeMap;
};