#include "stdafx.h"
#include "ModelKeyframe.h"

ModelKeyframe::ModelKeyframe()
{
}

ModelKeyframe::~ModelKeyframe()
{
}

D3DXMATRIX ModelKeyframe::GetInterpolatedMatrix(float time, AnimationPlayMode mode)
{
	UINT index1 = 0, index2 = 0;
	float interpolatedTime = 0.0f;

	CalcKeyframeIndex(time, mode, index1, index2, interpolatedTime);

	return GetInterpolatedMatrix(index1, index2, interpolatedTime);
}

UINT ModelKeyframe::GetKeyframeIndex(float time)
{
	UINT start = 0, end = frameCount - 1;

	if (time >= transforms[end].Time)
		return end; // �÷��̰� ����� ���̱� ������ end�� ����

	do
	{
		// Time�� ���� ��� ����ִ����� ã�� ��.
		UINT middle = (start + end) / 2;

		if (end - start <= 1)
			break;
		else if (transforms[middle].Time < time)
		{
			start = middle;
		}
		else if (transforms[middle].Time > time)
		{
			end = middle;
		}
		else
		{
			start = middle;
			break;
		}

	} while ((end - start) > 1);

	return start;
}

void ModelKeyframe::CalcKeyframeIndex(float time, AnimationPlayMode mode, OUT UINT & index1, OUT UINT & index2, OUT float & interpolatedTime)
{
	//index1 ���� �������� �ε���
	//index2 ���� �������� �ε���

	index1 = index2 = 0;
	interpolatedTime = 0.0f;

	index1 = GetKeyframeIndex(time);

	if (mode == AnimationPlayMode::Once)
		index2 = (index1 >= frameCount - 1) ? index1 : index1 + 1;
	else if(mode == AnimationPlayMode::Repeat)
		index2 = (index1 >= frameCount - 1) ? 0 : index1 + 1;

	if (index1 >= frameCount - 1)
	{
		index1 = index2 = frameCount - 1;
		interpolatedTime = 1.0f;
	}
	else
	{
		float time1 = time - transforms[index1].Time;
		float time2 = transforms[index2].Time - transforms[index1].Time;
		interpolatedTime = time1 / time2; // time1�� time2 ������ �ð� ������ ����.
	}
}

D3DXMATRIX ModelKeyframe::GetInterpolatedMatrix(UINT index1, UINT index2, float t)
{
	D3DXVECTOR3 scale;
	D3DXMATRIX S;
	D3DXVec3Lerp(&scale, &transforms[index1].S, &transforms[index1].S, t);
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);

	D3DXQUATERNION q;
	D3DXMATRIX R;
	D3DXQuaternionSlerp(&q, &transforms[index1].R, &transforms[index2].R, t);
	D3DXMatrixRotationQuaternion(&R, &q);

	D3DXVECTOR3 translation;
	D3DXMATRIX T;
	D3DXVec3Lerp(&translation, &transforms[index1].T, &transforms[index1].T, t);
	D3DXMatrixTranslation(&T, translation.x, translation.y, translation.z);

	return S * R * T;
}
