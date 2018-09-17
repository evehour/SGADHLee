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
	class ModelClip* current; // 현재 동작
	class ModelClip* next; // 다음 동작
	
	float blendTime;
	float elapsedTime;
};