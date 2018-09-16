#include "stdafx.h"
#include "ModelClip.h"
#include "../Utilities/BinaryFile.h"

ModelClip::ModelClip(wstring file)
	: playTime(0.0f)
	, bRepeat(false), speed(1.0f), bLockRoot(false)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	name = String::ToWString(r->String());
	duration = r->Float();
	frameRate = r->Float();
	frameCount = r->UInt();

	UINT keyframesCount = r->UInt();
	for (UINT i = 0; i < keyframesCount; i++)
	{
		ModelKeyframe* keyframe = new ModelKeyframe();
		keyframe->boneName = String::ToWString(r->String());

		keyframe->duration = duration;
		keyframe->frameCount = frameCount;
		keyframe->frameRate = frameRate;


		UINT size = r->UInt();
		if (size > 0)
		{
			keyframe->transforms.assign(size, ModelKeyframe::Transform());

			void* ptr = (void *)&keyframe->transforms[0];
			r->Byte(&ptr, sizeof(ModelKeyframe::Transform) * size);
		}

		keyframeMap.insert(Pair(keyframe->boneName, keyframe));
	}

	r->Close();
	SAFE_DELETE(r);
}

ModelClip::~ModelClip()
{
	for (Pair keyframe : keyframeMap)
		SAFE_DELETE(keyframe.second);
}

void ModelClip::Reset()
{
	bRepeat = false;
	speed = 1.0f;
	playTime = 0.0f;
}

D3DXMATRIX ModelClip::GetKeyframeMatrix(ModelBone * bone, float time)
{
	wstring boneName = bone->Name();
	unordered_map<wstring, ModelKeyframe *>::iterator it;
	if ((it = keyframeMap.find(boneName)) == keyframeMap.end())
	{
		D3DXMATRIX temp;
		D3DXMatrixIdentity(&temp);

		return temp;
	}


	ModelKeyframe* keyframe = keyframeMap.at(boneName);

	playTime += speed * time;
	if (bRepeat == true)
	{
		if (playTime >= duration)
		{
			// 나눈 나머지를 떨어뜨리게하는 연산인데, float은 나머지 연산이 되지 않으므로 이렇게 함.
			while (playTime - duration >= 0)
				playTime -= duration;
		}
	}
	else
	{
		if (playTime >= duration)
			playTime = duration;
	}

	//TODO: 설명 다시 해준다고함.
	// 월드행렬을 역행렬 취하면 로컬행렬이 된다.
	D3DXMATRIX invGlobal = bone->Global();
	D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

	D3DXMATRIX animation = keyframe->GetInterpolatedMatrix(playTime, bRepeat);

	// 계층형 애니메이션의 skined animation인데,
	// 본의 위치를 재계산 해줘야함.

	D3DXMATRIX parent;
	int parentIndex = bone->ParentIndex();
	if (parentIndex < 0)
	{
		if (bLockRoot == true)
			D3DXMatrixIdentity(&parent);
		else
			parent = animation;
	}
	else
	{
		parent = animation * bone->Parent()->Global();
	}

	return invGlobal * parent;
}

void ModelClip::UpdateKeyframe(ModelBone * bone, float time)
{
	D3DXMATRIX animation = GetKeyframeMatrix(bone, time);

	bone->Local(animation);
}
