#include "stdafx.h"
#include "ModelClip.h"
#include "../Utilities/BinaryFile.h"

ModelClip::ModelClip(wstring file, float startTime)
	: playTime(startTime)
	, bRepeat(false), speed(1.0f)
	, bLockRoot(false)
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
		keyframe->frameRate = frameRate;
		keyframe->frameCount = frameCount;

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

void ModelClip::GetKeyframeMatrix(ModelBone * bone, float time)
{
	wstring boneName = bone->Name();
	unordered_map<wstring, ModelKeyframe *>::iterator it;

	if ((it = keyframeMap.find(boneName)) == keyframeMap.end())
		return;
	
	ModelKeyframe* keyframe = keyframeMap.at(boneName);
	playTime += speed * time;
	if (bRepeat)
	{
		if (playTime >= duration)
		{
			// ���� �������� ����߸����ϴ� �����ε�, float�� ������ ������ ���� �����Ƿ� �̷��� ��.
			while (playTime - duration >= 0)
				playTime -= duration;
		}
	}
	else
	{
		if (playTime >= duration)
			playTime = duration;
	}

	//TODO: ���� �ٽ� ���شٰ���.
	// ��������� ����� ���ϸ� ��������� �ȴ�.
	D3DXMATRIX invGlobal = bone->Global();
	D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

	D3DXMATRIX animation = keyframe->GetInterpolatedMatrix(playTime, bRepeat);

	// ������ �ִϸ��̼��� skined animation�ε�,
	// ���� ��ġ�� ���� �������.

	D3DXMATRIX parent;
	int parentIndex = bone->ParentIndex();
	if (parentIndex < 0)
	{
		if (bLockRoot)
			D3DXMatrixIdentity(&parent);
		else
			parent = animation;
	}
	else
	{
		parent = animation * bone->Parent()->Global();
	}

	bone->Local(invGlobal * parent);
}
