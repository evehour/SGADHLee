#pragma once

class ModelTweener
{
public:
	ModelTweener();
	~ModelTweener();

	void Play
	(
		class ModelClip* clip
		, bool bRepeat
		, float blendTime = 0.0f
		, float speed = 1.0f
		, float startTime = 0.0f
	);

	void UpdateBlending(class ModelBone* bone, float time, bool isDelta = true);

private:
	class ModelClip* current; // ���� ����
	class ModelClip* next; // ���� ����
	
	float blendTime;
	float elapsedTime;
};