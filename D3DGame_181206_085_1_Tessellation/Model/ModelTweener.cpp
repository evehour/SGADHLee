#include "stdafx.h"
#include "../Model/ModelBone.h"
#include "ModelTweener.h"
#include "ModelClip.h"

ModelTweener::ModelTweener()
	: current(NULL), next(NULL), blendTime(0.0f), elapsedTime(0.0f)
{
	
}

ModelTweener::~ModelTweener()
{
}

void ModelTweener::Play(ModelClip * clip, bool bRepeat, float blendTime, float speed, float startTime)
{
	this->blendTime = blendTime;
	this->elapsedTime = startTime;
	clip->TriggerInit();

	if (current == NULL)
	{
		current = clip;
		current->Repeat(bRepeat);
		current->StartTime(startTime);
		current->Speed(speed);
	}

	if (next == NULL)
	{
		next = clip;
		next->Repeat(bRepeat);
		next->StartTime(startTime);
		next->Speed(speed);
	}
	else
	{
		current = next;
		next = clip;
		next->Repeat(bRepeat);
		next->StartTime(startTime);
		next->Speed(speed);
	}
}

void ModelTweener::UpdateBlending(ModelBone * bone, float time, bool isDelta)
{
	D3DXMATRIX matrix;
	if (next != NULL)
	{
		elapsedTime += time;

		//여기가 핵심
		float t;
		t = (isDelta ? elapsedTime : time) / blendTime;

		if (t > 1.0f)
		{
			matrix = next->GetKeyframeMatrix(bone, time, isDelta);

			current = next;
			next = NULL;
		}
		else
		{
			D3DXMATRIX start = current->GetKeyframeMatrix(bone, time, isDelta);
			D3DXMATRIX end = next->GetKeyframeMatrix(bone, time, isDelta);

			Math::LerpMatrix(matrix, start, end, t); // 다시 보자
		}
	}
	else
	{
		matrix = current->GetKeyframeMatrix(bone, time, isDelta);
	}

	bone->Local(matrix);
}

void ModelTweener::TriggerCheck()
{
	if (current)
		current->TriggerCheck();
}
