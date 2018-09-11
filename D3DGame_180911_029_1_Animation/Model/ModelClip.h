#pragma once

class ModelClip
{
public:
	ModelClip(wstring file);
	~ModelClip();

private:
	wstring name;

	float duration;
	float frameRate;
	UINT frameCount;

	vector<class ModelKeyframe *> keyframes;
};