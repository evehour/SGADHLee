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

	UINT keyframesCount = r->UInt(); // 65
	for (UINT i = 0; i < keyframesCount; i++)
	{
		ModelKeyframe* keyframe = new ModelKeyframe();
		keyframe->boneName = String::ToWString(r->String());

		keyframe->duration = duration;
		keyframe->frameCount = frameCount;
		keyframe->frameRate = frameRate;


		UINT size = r->UInt(); // 20
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

	ZeroMemory(changedName, 1024);
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

D3DXMATRIX ModelClip::GetKeyframeMatrix(ModelBone * bone, float time, bool isDelta)
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

	if (isDelta) playTime += speed * time;
	else playTime = time;

	if (bRepeat == true)
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

D3DXMATRIX ModelClip::GetKeyframeOriginMatrix(ModelBone * bone, float time, bool isDelta)
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

	if (isDelta) playTime += speed * time;
	else playTime = time;

	if (bRepeat == true)
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

	return keyframe->GetInterpolatedMatrix(playTime, bRepeat);
}

void ModelClip::UpdateKeyframe(ModelBone * bone, float time)
{
	D3DXMATRIX animation = GetKeyframeMatrix(bone, time);

	bone->Local(animation);
}

void ModelClip::WriteClip(wstring file)
{
	wstring directory = Path::GetDirectoryName(file);
	Path::CreateFolders(directory);

	BinaryWriter* w = new BinaryWriter();
	w->Open(file);

	w->String(String::ToString(name));
	w->Float(duration);
	w->Float(frameRate);
	w->UInt(frameCount);

	w->UInt(keyframeMap.size());

	unordered_map<wstring, ModelKeyframe *>::iterator it;
	it = keyframeMap.begin();

	while(it != keyframeMap.end())
	{
		w->String(String::ToString((*it).second->boneName));
		UINT size = (*it).second->transforms.size();

		w->UInt(size);
		if (size > 0)
		{
			w->Byte(&(*it).second->transforms[0], sizeof(ModelKeyframe::Transform) * (*it).second->transforms.size());
		}

		it++;
	}

	w->Close();
	SAFE_DELETE(w);
}

void ModelClip::AddMotion(ModelBone * bone, ModelKeyframe::Transform tf, bool incGlobalFrameCount)
{
	if (incGlobalFrameCount)
	{
		frameCount++;
	}
	else
	{
		if (keyframeMap.find(bone->Name()) == keyframeMap.end()) return;
		keyframeMap.at(bone->Name())->transforms.push_back(tf);
		keyframeMap.at(bone->Name())->OrderingTransform();
		keyframeMap.at(bone->Name())->frameCount = keyframeMap.at(bone->Name())->frameCount + 1;
	}
}

void ModelClip::GetKeyframeTransform(ModelBone * bone, OUT vector<ModelKeyframe::Transform>& vec)
{
	vec.assign(keyframeMap.at(bone->Name())->transforms.begin(), keyframeMap.at(bone->Name())->transforms.end() - 1);
}

void ModelClip::EditAnimBoneName()
{
	int i = 0;
	unordered_map<wstring, ModelKeyframe *> tempMap;
	unordered_map<wstring, ModelKeyframe *>::iterator it;
	bool isConfirm = false;
	it = keyframeMap.begin();

	while (it != keyframeMap.end())
	{
		string tmp = "Change bone name##" + to_string(i);
		ImGui::Text(String::ToString(it->first).c_str());

		if (ImGui::IsItemClicked(1))
		{
			sprintf_s(changedName, "%s", String::ToString(it->second->boneName).c_str());
			ImGui::OpenPopup(tmp.c_str());
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("STMB");
			if (payload != NULL)
			{
				IM_ASSERT(payload->DataSize == sizeof(ModelBone));
				ModelBone* data = (ModelBone*)payload->Data;
				sprintf_s(changedName, "%s", String::ToString(data->Name()).c_str());
				isConfirm = true;
				clickedBoneNamed = it->first;
			}
		}

		if (ImGui::BeginPopupModal(tmp.c_str()))
		{
			ImGui::InputText("Bone name", changedName, 1024);

			if (ImGui::Button("Confirm"))
			{
				clickedBoneNamed = it->first;
				isConfirm = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close"))
			{ 
				ImGui::CloseCurrentPopup();
			}
			ImGui::TextWrapped("                                               ");

			ImGui::EndPopup();
		}//ImGui::Modal
		i++;
		it++;
	}//while

	if (isConfirm)
	{
		unordered_map<wstring, ModelKeyframe *> tempMap;
		it = keyframeMap.begin();
		wstring _name = String::ToWString(changedName);

		while (it != keyframeMap.end())
		{
			if (it->first == clickedBoneNamed)
			{
				it->second->boneName = _name;
				tempMap.insert(Pair(_name, it->second));
			}
			else
			{
				tempMap.insert(Pair(it->first, it->second));
			}

			it++;
		}

		keyframeMap.clear();
		keyframeMap = tempMap;
	}
}