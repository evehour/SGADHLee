#include "stdafx.h"
#include "BoneAnimaitionTool.h"

#include "../Utilities/Xml.h"

#include "../Objects/GameModel.h"

BoneAnimationTool::BoneAnimationTool(ExecuteValues* values, GameModel * model)
	: values(values), targetModel(model)
	, playSpeedRate(1.0f), currentSec(0.0f)
{
	pickedPartName = "None";

	targetModel->Update();
	SaveClip();
	pickedData.BoneIdx = -1;

	isPlay = false;
	isRepeat = false;
}

BoneAnimationTool::~BoneAnimationTool()
{
}

void BoneAnimationTool::Update()
{
	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);

	D3DXVECTOR3 direction = values->MainCamera->Direction(
		values->Viewport, values->Perspective
	);

	if (Keyboard::Get()->Press(VK_LSHIFT) && Mouse::Get()->Down(0))
	{
		if (targetModel->IsPickPart(start, direction, pickedData.BoneIdx, NULL))
		{
			pickedPartName = String::ToString(targetModel->GetModel()->BoneByIndex(pickedData.BoneIdx)->Name());
			D3DXMATRIX pickLocal = targetModel->GetModel()->BoneByIndex(pickedData.BoneIdx)->Local();
			D3DXMatrixDecompose(&pickedData.Scale, &pickedData.qRotation, &pickedData.Position, &pickLocal);

			double x, y, z;
			toEulerAngle(pickedData.qRotation, x, y, z);
			pickedData.Rotation = D3DXVECTOR3((float)x, (float)y, (float)z);
		}
		else
		{
			pickedData.BoneIdx = -1;
			pickedPartName = "None";
		}
	}

	if (isPlay) SettingPose();
	else
	{
		SettingStopPose(pickedData.BoneIdx);
	}

	targetModel->Update();
	return;
}

void BoneAnimationTool::Render()
{
	targetModel->Render();
	return;
}

void BoneAnimationTool::PostRender()
{
	ImGui::Begin("BoneAnimaiton Tool");
	{
		ImGui::Text(("Current Pick Part: " + pickedPartName).c_str());
		ImGui::Separator();

		if (pickedData.BoneIdx >= 0)
		{
			ImGui::Text("Part transform");
			ImGui::SliderFloat3("Part Scale", (float *)&pickedData.Scale, 0.0f, 10.0f);
			ImGui::SliderFloat3("Part Rot", (float *)&pickedData.Rotation, -Math::PI, Math::PI);
			ImGui::SliderFloat3("Part Pos", (float *)&pickedData.Position, -500.0f, 500.0f);

			D3DXMATRIX F, S, R, T;
			D3DXMatrixScaling(&S, pickedData.Scale.x, pickedData.Scale.y, pickedData.Scale.z);
			D3DXQuaternionRotationYawPitchRoll(&pickedData.qRotation, pickedData.Rotation.y, pickedData.Rotation.x, pickedData.Rotation.z);
			D3DXMatrixRotationQuaternion(&R, &pickedData.qRotation);
			D3DXMatrixTranslation(&T, pickedData.Position.x, pickedData.Position.y, pickedData.Position.z);
			F = S * R * T;
			targetModel->GetModel()->BoneByIndex(pickedData.BoneIdx)->Local(F);
		}
		ImGui::Separator();

		ImGui::SliderFloat("Speed Rate", &playSpeedRate, 0.0f, 20.0f, "%.6f");
		ImGui::SliderFloat("Current Sec", &currentSec, 0.0f, 1.0f, "%.6f");
		ImGui::Separator();
	}
	ImGui::End();

	ImGui::Begin("BoneAnimation Controller");
	{
		if (ImGui::Button("Play"))
		{
			isPlay = true;
			currentSec = (currentSec >= 1.0f) ? 0.0f : currentSec;
			currentPose = clips.begin();
			nextPose = (currentPose + 1);
		}
		ImGui::SameLine();
		ImGui::Checkbox("Repeat", &isRepeat);

		if (ImGui::Button("Clip!"))
		{
			SaveClip();
		}

		ImGui::Separator();
		ImGui::Separator();

		if (ImGui::Button("Save File"))
		{
			Path::SaveFileDialog(L"", L"XML Files(*.xml)\0*.xml\0", Textures, bind(&BoneAnimationTool::SaveFile, this, placeholders::_1));
		}

		if (ImGui::Button("Load File"))
		{
			Path::OpenFileDialog(L"", L"XML Files(*.xml)\0*.xml\0", Textures, bind(&BoneAnimationTool::LoadFile, this, placeholders::_1));
		}
	}
	ImGui::End();

	//ImGui::Begin("BoneAnimaiton Clip");
	//{
	//	ImGui::Text(("Current Pick Part: " + pickedPartName).c_str());	ImGui::SameLine();	ImGui::Button("Select");
	//}
	//ImGui::End();
	return;
}

void BoneAnimationTool::SaveFile(wstring fName)
{
	wstring directory = Path::GetDirectoryName(fName);
	Path::CreateFolders(directory);

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLDeclaration *decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("AnimationClips");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	Xml::XMLElement* inform = document->NewElement("Information");
	root->LinkEndChild(inform);

	Xml::XMLElement* informElement = NULL;
	informElement = document->NewElement("Count");
	informElement->SetText(clips.size());
	inform->LinkEndChild(informElement);

	informElement = document->NewElement("SpeedRate");
	informElement->SetText(playSpeedRate);
	inform->LinkEndChild(informElement);

	informElement = document->NewElement("BoneCount");
	informElement->SetText(targetModel->GetModel()->BoneCount());
	inform->LinkEndChild(informElement);

	Xml::XMLElement* clipsRoot = document->NewElement("Clips");
	root->LinkEndChild(clipsRoot);

	for (UINT i = 0; i < clips.size(); i++)
	{
		Xml::XMLElement* clipRoot = document->NewElement("Clip");
		clipsRoot->LinkEndChild(clipRoot);

		Xml::XMLElement* tr = document->NewElement("TimeRating");
		tr->SetText(clips[i].TimeRating);
		clipRoot->LinkEndChild(tr);

		for (UINT j = 0; j < targetModel->GetModel()->BoneCount(); j++)
		{
			Xml::XMLElement* boneDataRoot = document->NewElement("BoneData");
			clipRoot->LinkEndChild(boneDataRoot);

			Xml::XMLElement* boneInform = NULL;
			Xml::XMLElement* boneInformSub = NULL;

			boneInform = document->NewElement("BoneIdx");
			boneInform->SetText(clips[i].Datas[j].BoneIdx);
			boneDataRoot->LinkEndChild(boneInform);

			boneInform = document->NewElement("Scale");
			boneDataRoot->LinkEndChild(boneInform);
			boneInformSub = document->NewElement("X");
			boneInformSub->SetText(clips[i].Datas[j].Scale.x);
			boneInform->LinkEndChild(boneInformSub);
			boneInformSub = document->NewElement("Y");
			boneInformSub->SetText(clips[i].Datas[j].Scale.y);
			boneInform->LinkEndChild(boneInformSub);
			boneInformSub = document->NewElement("Z");
			boneInformSub->SetText(clips[i].Datas[j].Scale.z);
			boneInform->LinkEndChild(boneInformSub);

			boneInform = document->NewElement("Rotation");
			boneDataRoot->LinkEndChild(boneInform);
			boneInformSub = document->NewElement("X");
			boneInformSub->SetText(clips[i].Datas[j].Rotation.x);
			boneInform->LinkEndChild(boneInformSub);
			boneInformSub = document->NewElement("Y");
			boneInformSub->SetText(clips[i].Datas[j].Rotation.y);
			boneInform->LinkEndChild(boneInformSub);
			boneInformSub = document->NewElement("Z");
			boneInformSub->SetText(clips[i].Datas[j].Rotation.z);
			boneInform->LinkEndChild(boneInformSub);
			boneInformSub = document->NewElement("W");
			boneInformSub->SetText(clips[i].Datas[j].Rotation.w);
			boneInform->LinkEndChild(boneInformSub);

			boneInform = document->NewElement("Position");
			boneDataRoot->LinkEndChild(boneInform);
			boneInformSub = document->NewElement("X");
			boneInformSub->SetText(clips[i].Datas[j].Position.x);
			boneInform->LinkEndChild(boneInformSub);
			boneInformSub = document->NewElement("Y");
			boneInformSub->SetText(clips[i].Datas[j].Position.y);
			boneInform->LinkEndChild(boneInformSub);
			boneInformSub = document->NewElement("Z");
			boneInformSub->SetText(clips[i].Datas[j].Position.z);
			boneInform->LinkEndChild(boneInformSub);
		}
	}

	string file = String::ToString(fName);
	document->SaveFile(file.c_str());
}

void BoneAnimationTool::LoadFile(wstring fName)
{
	clips.clear();

	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLError error = document->LoadFile(String::ToString(fName).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* inform = root->FirstChildElement();
	Xml::XMLElement* node = NULL;
	node = inform->FirstChildElement();

	UINT clipCount = node->IntText();
	
	node = node->NextSiblingElement();
	playSpeedRate = node->FloatText();

	node = node->NextSiblingElement();
	UINT boneCount = node->IntText();

	Xml::XMLElement* clipRoot = NULL;
	clipRoot = inform->NextSiblingElement();
	Xml::XMLElement* clip = clipRoot->FirstChildElement();

	for (UINT i = 0; i < clipCount; i++)
	{
		AnimationData aData;

		node = clip->FirstChildElement();
		aData.TimeRating = node->FloatText();

		Xml::XMLElement* boneData = node->NextSiblingElement();
		
		for (UINT j = 0; j < boneCount; j++)
		{
			BoneData bData;

			Xml::XMLElement* boneDataNode = boneData->FirstChildElement();
			bData.BoneIdx = boneDataNode->IntText();

			boneDataNode = boneDataNode->NextSiblingElement();
			Xml::XMLElement* boneDataNodeSub = NULL;
			
			boneDataNodeSub = boneDataNode->FirstChildElement();
			bData.Scale.x = boneDataNodeSub->FloatText();
			boneDataNodeSub = boneDataNodeSub->NextSiblingElement();
			bData.Scale.y = boneDataNodeSub->FloatText();
			boneDataNodeSub = boneDataNodeSub->NextSiblingElement();
			bData.Scale.z = boneDataNodeSub->FloatText();

			boneDataNode = boneDataNode->NextSiblingElement();
			boneDataNodeSub = boneDataNode->FirstChildElement();
			bData.Rotation.x = boneDataNodeSub->FloatText();
			boneDataNodeSub = boneDataNodeSub->NextSiblingElement();
			bData.Rotation.y = boneDataNodeSub->FloatText();
			boneDataNodeSub = boneDataNodeSub->NextSiblingElement();
			bData.Rotation.z = boneDataNodeSub->FloatText();
			boneDataNodeSub = boneDataNodeSub->NextSiblingElement();
			bData.Rotation.w = boneDataNodeSub->FloatText();

			boneDataNode = boneDataNode->NextSiblingElement();
			boneDataNodeSub = boneDataNode->FirstChildElement();
			bData.Position.x = boneDataNodeSub->FloatText();
			boneDataNodeSub = boneDataNodeSub->NextSiblingElement();
			bData.Position.y = boneDataNodeSub->FloatText();
			boneDataNodeSub = boneDataNodeSub->NextSiblingElement();
			bData.Position.z = boneDataNodeSub->FloatText();

			aData.Datas.push_back(bData);
			boneData = boneData->NextSiblingElement();
		}

		clips.push_back(aData);
		clip = clip->NextSiblingElement();
	}
}

void BoneAnimationTool::SaveClip()
{
	AnimationData animationData;
	animationData.TimeRating = currentSec / playSpeedRate;
	animationData.Datas.clear();
	animationData.Datas.reserve(targetModel->GetModel()->BoneCount());

#if true
	for (ModelBone* bone : targetModel->GetModel()->Bones())
	{
		BoneData boneData;
		D3DXMATRIX local = bone->Local();

		boneData.BoneIdx = bone->Index();
		D3DXMatrixDecompose(&boneData.Scale, &boneData.Rotation, &boneData.Position, &local);

		animationData.Datas.push_back(boneData);
	}
#else
	for (UINT i = 0; i < targetModel->GetModel()->BoneCount(); i++)
	{
		BoneData boneData;
		boneData.BoneIdx = i;
		D3DXMATRIX local = targetModel->GetModel()->Bones()[i]->Local();
		animationData.Datas.push_back(boneData);
	}
#endif
	clips.push_back(animationData);

	std::sort(clips.begin(), clips.end());
	
	if (clips.size() > 1)
	{
		if (clips[0].TimeRating == clips[1].TimeRating)
		{
			clips.erase(clips.begin());
		}

		if (clips.size() > 2) {
			if (clips[clips.size() - 1].TimeRating == clips[clips.size() - 2].TimeRating)
			{
				clips.erase(clips.begin() + (clips.size() - 1));
			}
		}
	}
	return;
}

void BoneAnimationTool::SettingPose()
{
	float t = currentSec / 1.0f;
	float dt = 0.0f;
	if (nextPose != clips.end()) {
		dt = (t - (*currentPose).TimeRating) / ((*nextPose).TimeRating - (*currentPose).TimeRating);

		if (dt >= 1.0f)
		{
			currentPose = nextPose;
			nextPose = (nextPose + 1);
		}
		LerpPart(currentPose, nextPose, dt);
	}
	else if(isRepeat)
	{
		vector<AnimationData>::iterator tmp = nextPose;
		if (tmp == clips.end()) tmp = clips.begin();

		if ((*currentPose).TimeRating < (*tmp).TimeRating)
		{
			dt = (t - (*currentPose).TimeRating) / ((*tmp).TimeRating - (*currentPose).TimeRating);

			if (dt >= 1.0f)
			{
				currentPose = nextPose;
				nextPose = (nextPose + 1);
			}
			LerpPart(currentPose, nextPose, dt);
		}
		else
		{
			dt = (t - (*currentPose).TimeRating) / (1.0f - (*currentPose).TimeRating);
			LerpPart(currentPose, clips.begin(), dt);
			if (dt >= 1.0f)
			{
				currentPose = nextPose;
				nextPose = (nextPose + 1);
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////

	currentSec += playSpeedRate * Time::Delta();
	bool condition = true;
	if (currentSec >= 1.0f) { StopAnimation(); return; }

	return;
}

void BoneAnimationTool::SettingStopPose(int boneIdx)
{
	vector<AnimationData>::iterator tmp;
	float t = currentSec / 1.0f;
	float dt = 0.0f;
	int idx;

	if (boneIdx < 0)
	{
		if (clips.size() < 2) return;

		nextPose = clips.end();
		for (tmp = clips.begin(); tmp != clips.end(); tmp++)
		{
			if ((*tmp).TimeRating > t)
			{
				currentPose = (tmp - 1);
				nextPose = tmp;
				break;
			}
		}
		if (nextPose == clips.end()) return;
		dt = (t - (*currentPose).TimeRating) / ((*nextPose).TimeRating - (*currentPose).TimeRating);
		LerpPart(currentPose, nextPose, dt);
	}
	else
	{
#if false
		for (tmp = clips.begin(); tmp != clips.end(); tmp++)
		{
			if ((*tmp).TimeRating > t)
			{
				currentPose = (tmp - 1);
				nextPose = tmp;

				pickedData.Scale = (*currentPose).Datas[boneIdx].Scale;
				pickedData.Position = (*currentPose).Datas[boneIdx].Position;

				double x, y, z;
				D3DXQUATERNION qR;
				pickedData.qRotation = (*currentPose).Datas[boneIdx].Rotation;
				toEulerAngle((*currentPose).Datas[boneIdx].Rotation, x, y, z);
				pickedData.Rotation = D3DXVECTOR3((float)x, (float)y, (float)z);

				break;
			}
		}
#endif
	}
}

void BoneAnimationTool::StopAnimation()
{
	if (!isRepeat)
	{
		currentSec = 1.0f;
		isPlay = false;
	}
	else
	{
		currentSec = 0.0f;
		//currentPose = (clips.begin() + (clips.size() - 1));
		currentPose = (clips.end() - 1);
		nextPose = (clips.begin());
	}
	return;
}

void BoneAnimationTool::LerpPart(vector<AnimationData>::iterator a, vector<AnimationData>::iterator b, float & t)
{
	D3DXMATRIX S, R, T, F;
	D3DXVECTOR3 vS, vT;
	D3DXQUATERNION qR;

	if (b != clips.end())
	{
		for (UINT i = 0; i < targetModel->GetModel()->BoneCount(); i++)
		{
			D3DXVec3Lerp(&vS, &(a->Datas[i].Scale), &(b->Datas[i].Scale), t);
			D3DXQuaternionSlerp(&qR, &(a->Datas[i].Rotation), &(b->Datas[i].Rotation), t);
			D3DXVec3Lerp(&vT, &(a->Datas[i].Position), &(b->Datas[i].Position), t);

			D3DXMatrixScaling(&S, vS.x, vS.y, vS.z);
			D3DXMatrixRotationQuaternion(&R, &qR);
			F = S * R;
			F._41 = vT.x;
			F._42 = vT.y;
			F._43 = vT.z;

			targetModel->GetModel()->Bones()[i]->Local(F);
		}
	}
}


