#pragma once

// enum에 class를 안붙이면 전역으로 취급되며, 붙이면 구조체처럼 지역에서 쓰는것처럼 됨.
enum class AnimationPlayMode
{
	Once = 0, Repeat,
};

class ModelKeyframe
{
public:
	struct Transform;
	friend class ModelClip;

	ModelKeyframe();
	~ModelKeyframe();

public:
	D3DXMATRIX GetInterpolatedMatrix(float time, AnimationPlayMode mode);

private:
	UINT GetKeyframeIndex(float time); // delta time이 들어올거임.
	void CalcKeyframeIndex(
		float time, AnimationPlayMode mode
		, OUT UINT& index1, OUT UINT& index2, OUT float& interpolatedTime);
	D3DXMATRIX GetInterpolatedMatrix(UINT index1, UINT index2, float t);


private:
	wstring boneName;

	float duration;
	float frameRate;
	UINT frameCount;

	vector<Transform> transforms;

public:
	struct Transform
	{
		float Time;

		D3DXVECTOR3 S;
		D3DXQUATERNION R;
		D3DXVECTOR3 T;
	};
};