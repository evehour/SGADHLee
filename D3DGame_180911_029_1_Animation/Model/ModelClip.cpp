#include "stdafx.h"
#include "ModelClip.h"
#include "ModelKeyframe.h"
#include "../Utilities/BinaryFile.h"

ModelClip::ModelClip(wstring file)
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
	}

	r->Close();
	SAFE_DELETE(r);
}

ModelClip::~ModelClip()
{
	for (ModelKeyframe* keyframe : keyframes)
		SAFE_DELETE(keyframe);
}
