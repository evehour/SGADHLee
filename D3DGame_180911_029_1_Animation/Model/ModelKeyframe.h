#pragma once

// enum�� class�� �Ⱥ��̸� �������� ��޵Ǹ�, ���̸� ����üó�� �������� ���°�ó�� ��.
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
	UINT GetKeyframeIndex(float time); // delta time�� ���ð���.
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