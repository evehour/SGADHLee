#include "stdafx.h"
#include "TestAnimationTool.h"
#include "../Objects/GameAnimModel.h"

#include "../Fbx/Exporter.h"
#include "../Model/ModelClip.h"
#include "../Objects/DebugDraw.h"

TestAnimationTool::TestAnimationTool(ExecuteValues * values)
	: Execute(values)
	, targetModel(nullptr)
	, animExportIdxArray(nullptr)
	, bOpenFbxAttr(false), bFileLoadComplete(false)
	, attrDataFlag(0), extDataFlag(0)
	, selectedTargetBone(-1), selectedAnimationClipsIdx(-1), selectedAnimationIdx(-1)
	, animationTime(0.0f), isRepeat(false), isPlayAnimation(false), animationPlaySpeed(20.0f)
	, isSaveAnimDataWindowOpen(false)
	, m_S(1.0f, 1.0f, 1.0f), m_R(0.0f, 0.0f, 0.0f), m_qR(0.0f, 0.0f, 0.0f, 0.0f), m_T(0.0f, 0.0f, 0.0f)
{

	depthStencilState[0] = new DepthStencilState();
	depthStencilState[1] = new DepthStencilState();
	depthStencilState[1]->DepthEnable(false);

	models.clear();
	clips.clear();
}

TestAnimationTool::~TestAnimationTool()
{
	for (UINT i = 0; i < models.size(); i++)
		SAFE_DELETE(models[i]);

	for (UINT i = 0; i < clips.size(); i++)
		SAFE_DELETE(clips[i]);
	
	for (UINT i = 0; i < bonePin.size(); i++)
		SAFE_DELETE(bonePin[i]);
}

void TestAnimationTool::Update()
{
	if (targetModel != nullptr)
	{
		// 애니메이션 재생 부분
		if (selectedAnimationIdx > -1 && isPlayAnimation)
		{
			animationTime += animationPlaySpeed * Time::Delta();
			if (animationTime >= targetModel->GetClipDuration(selectedAnimationIdx))
			{
				if (isRepeat)
				{
					animationTime = 0.0f;
				}
				else
				{
					animationTime = targetModel->GetClipDuration(selectedAnimationIdx);
					isPlayAnimation = false;
				}
			}
		}//if(selectedAnimation && isPlayAnimation

		targetModel->SetTime(animationTime);

		if ((selectedTargetBone > -1) && (animationBeforeTime != animationTime))
		{
			bool b = true;
			targetModel->SetAnimMatrixUpdate(b);
			animationBeforeTime = animationTime;

			D3DXMATRIX dummy;
			ExportData(m_S, m_qR, m_R, m_T, dummy, animationTime);
		}
		else
		{
			targetModel->SetAnimMatrixUpdate(isPlayAnimation);
		}

		Model *model = targetModel->GetModel();

		if (Mouse::Get()->Down(0))
		{
			D3DXVECTOR3 start;
			values->MainCamera->Position(&start);

			D3DXVECTOR3 direction = values->MainCamera->Direction(
				values->Viewport, values->Perspective
			);
			D3DXVECTOR3 tmp;

			for (UINT i = 0; i < bonePin.size(); i++)
			{
				if (bonePin[i]->IsPick(start, direction, tmp))
				{
					selectedTargetBone = i;
					break;
				}
			}
		}
		
		targetModel->Update();

		for (UINT i = 0; i < bonePin.size(); i++)
		{
			ModelBone* b = model->Bones()[i];
			D3DXMATRIX m = b->Global() * targetModel->GetBoneTransforms()[i];

			if (selectedTargetBone == i)
			{
				if (isPlayAnimation)
				{
					D3DXMATRIX dummy;
					ExportData(m_S, m_qR, m_R, m_T, dummy, animationTime);
				}
			}

			bonePin[i]->Position(m._41, m._42, m._43);
			if (i == selectedTargetBone)
			{
				bonePin[i]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
			}
			else
			{
				bonePin[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			}
			bonePin[i]->Update();
		}
	}//if(targetModel != nullptr)
}

void TestAnimationTool::PreRender()
{

}

void TestAnimationTool::Render()
{
	if (targetModel != nullptr)
	{
		targetModel->Render();

		depthStencilState[1]->OMSetDepthStencilState();
		for (UINT i = 0; i < bonePin.size(); i++)
		{
			bonePin[i]->Render();
		}
		depthStencilState[0]->OMSetDepthStencilState();
	}
}

void TestAnimationTool::PostRender()
{
#if false
	ImGui::Begin("Test");
	{
		ImGui::SetWindowSize(ImVec2(200, 668));
		ImGui::SetWindowPos(ImVec2(16, 34));
	}
	ImGui::End();
#endif

	if (ImGui::BeginMainMenuBar())
	{
		MainMenu();
		ImGui::EndMainMenuBar();
	}

	if (isSaveAnimDataWindowOpen)
		ImGui::OpenPopup("AnimationSave");

	if (ImGui::BeginPopupModal("AnimationSave"))
	{
		UINT size = clips.size();
		if (size > 0)
		{
			char** _anims;
			_anims = new char*[clips.size()];
			for (UINT i = 0; i < size; i++)
			{
				_anims[i] = new char[String::ToString(clips[i]->GetName()).length() + 1];
				strcpy_s(_anims[i], String::ToString(clips[i]->GetName()).length() + 1, String::ToString(clips[i]->GetName()).c_str());
			}

			ImGui::ListBox("Animation\nList", &saveAnimListIdx, _anims, size);

			if (ImGui::Button("Save"))
			{
				Path::SaveFileDialog
				(
					L""
					, L"Animation Files(*.anim)\0*.anim\0"
					, Assets
					, bind(&ModelClip::WriteClip, clips[saveAnimListIdx], placeholders::_1)
				);
			}

			ImGui::SameLine(0.0f, 28.0f);

			if (ImGui::Button("Close"))
			{
				isSaveAnimDataWindowOpen = false;
				ImGui::CloseCurrentPopup();
			}

			for (UINT i = 0; i < clips.size(); i++)
				SAFE_DELETE(_anims[i]);
			SAFE_DELETE_ARRAY(_anims);
			ImGui::TextWrapped("                                               ");
		}
		else
		{
			isSaveAnimDataWindowOpen = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Pre-Extract process
	if(bOpenFbxAttr)
	{
		ImGui::Begin("Export Option");
		{
			ExportOption();
		}
		ImGui::End();
	}

	// Post-Load
	if(bFileLoadComplete)
	{
		ImGui::Begin("Hireacy");
		{
			// Fixed window position
			{
				//ImGui::SetWindowSize(ImVec2(200, 668));
				//ImGui::SetWindowPos(ImVec2(16, 34));
			}
			ImguiHireacy();

		}//ImGui::Begin("Hireacy");
		ImGui::End();

		ImGui::Begin("Inspector");
		{
			// Fixed window position
			{
				ImGui::SetWindowPos(ImVec2(936, 34));
				ImGui::SetWindowSize(ImVec2(500, 668));

				ImguiInspector();
			}
		}
		ImGui::End();

		if (selectedAnimationIdx > -1)
		{
			ImGui::Begin("Time spector");
			{
				ImguiTimeSpector();
			}
			ImGui::End();
		}
	}
}

void TestAnimationTool::OpenFbxFile(wstring fName)
{
	bOpenFbxAttr = true;
	extDataFlag = 0;

	Fbx::Exporter* exporter = NULL;
	selectedFbxFileName = fName;

	exporter = new Fbx::Exporter(selectedFbxFileName);
	attrDataFlag = exporter->CheckIncludeData(animationNames);
	if (attrDataFlag & (UINT)Fbx::FBX_INCLUDE_DATA::FBX_Animation)
	{
		animExportIdxArray = new bool[animationNames.size()];
		memset(animExportIdxArray, false, animationNames.size());
	}
	SAFE_DELETE(exporter);

	return;
}

void TestAnimationTool::OpenAnimFile(wstring fName)
{
	ModelClip* clip = nullptr;
	wstring _name = Path::GetFileNameWithoutExtension(fName);
	clip = new ModelClip(fName);
	clip->SetName(_name);
	clips.push_back(clip);

	bFileLoadComplete = true;
}

void TestAnimationTool::ExtractFbx(bool isCancel)
{
	Fbx::Exporter* exporter = NULL;
	bOpenFbxAttr = false;

	if (isCancel) return;
	if (extDataFlag == (UINT)Fbx::FBX_INCLUDE_DATA::FBX_Animation)
	{
		bool b = false;
		for (UINT i = 0; i < animationNames.size(); i++)
		{
			if (animExportIdxArray[i])
			{
				b = true;
				break;
			}
		}
		if (!b) return;
	}

	wstring fileName = Path::GetFileNameWithoutExtension(selectedFbxFileName);
	wstring folderName = Path::GetDirectoryName(selectedFbxFileName);

	wstring a, b;
	a = folderName + fileName + L"/";

	if (extDataFlag & (UINT)Fbx::FBX_INCLUDE_DATA::FBX_Model)
	{
		exporter = new Fbx::Exporter(selectedFbxFileName);
		exporter->ExportMaterial(a, fileName + L".material");
		exporter->ExportMesh(a, fileName + L".mesh");
		SAFE_DELETE(exporter);

		GameAnimModel* animModel = new GameAnimModel(a, fileName + L".material", a, fileName + L".mesh");
		animModel->SetShader(new Shader(Shaders + L"035_Animation_HW.hlsl"));
		animModel->SetName(fileName);
		models.push_back(animModel);
	}

	if (extDataFlag & (UINT)Fbx::FBX_INCLUDE_DATA::FBX_Animation)
	{
		exporter = new Fbx::Exporter(selectedFbxFileName, false);
		for (UINT i = 0; i < animationNames.size(); i++)
		{
			if (animExportIdxArray[i])
			{
				ModelClip* clip = nullptr;
				b = fileName + L"_" + String::ToWString(animationNames[i]) + L".anim";
				exporter->ExportAnimation(a, b, i);
				clip = new ModelClip(a + b);
				clip->SetName(String::ToWString(animationNames[i]));
				clips.push_back(clip);
			}
		}
		SAFE_DELETE_ARRAY(animExportIdxArray);
		SAFE_DELETE(exporter);
	}

	bFileLoadComplete = true;

	return;
}

void TestAnimationTool::ExportData(OUT D3DXVECTOR3 & scale, OUT D3DXQUATERNION & qrot, OUT D3DXVECTOR3 & rot, OUT D3DXVECTOR3 & pos, OUT D3DXMATRIX localMatrix, const float& time)
{
	D3DXMATRIX boneMatrix;
	double x, y, z;

	if ((int)clips.size() <= selectedAnimationClipsIdx) return;

	boneMatrix =
		clips[selectedAnimationClipsIdx]->GetKeyframeOriginMatrix(
			targetModel->GetModel()->BoneByIndex(selectedTargetBone)
			, time
			, false
		);

	D3DXMatrixDecompose(&scale, &qrot, &pos, &boneMatrix);
	toEulerAngle(m_qR, x, y, z);
	rot.x = (float)x; rot.y = (float)y; rot.z = (float)z;

	D3DXMATRIX invGlobal, parent;
	invGlobal = targetModel->GetModel()->BoneByIndex(selectedTargetBone)->Global();
	D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

	int parentIndex = targetModel->GetModel()->BoneByIndex(selectedTargetBone)->ParentIndex();
	if (parentIndex < 0)
	{
		parent = boneMatrix;
	}
	else
	{
		parent = boneMatrix * targetModel->GetModel()->BoneByIndex(selectedTargetBone)->Parent()->Global();
	}

	localMatrix = invGlobal * parent;
}

void TestAnimationTool::CalcData(const D3DXVECTOR3 & scale, const D3DXVECTOR3 & rot, const D3DXVECTOR3 & pos, OUT D3DXMATRIX& localMatrix)
{
	D3DXMATRIX boneMatrix;

	D3DXMATRIX _matS, _matR, _matT;
	D3DXMatrixScaling(&_matS, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&_matR, rot.y, rot.x, rot.z);
	D3DXMatrixTranslation(&_matT, pos.x, pos.y, pos.z);

	boneMatrix = _matS * _matR * _matT;

	D3DXMATRIX invGlobal, parent;
	invGlobal = targetModel->GetModel()->BoneByIndex(selectedTargetBone)->Global();
	D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

	int parentIndex = targetModel->GetModel()->BoneByIndex(selectedTargetBone)->ParentIndex();
	if (parentIndex < 0)
	{
		parent = boneMatrix;
	}
	else
	{
		parent = boneMatrix * targetModel->GetModel()->BoneByIndex(selectedTargetBone)->Parent()->Global();
	}

	localMatrix = invGlobal * parent;
}

void TestAnimationTool::BoneTreeCreator(GameAnimModel* model, ModelBone* bone)
{
	if (ImGui::TreeNode(String::ToString(bone->Name()).c_str()))
	{
		if (ImGui::IsItemClicked(1) && targetModel == model)
		{
			if (selectedTargetBone == bone->Index())
			{
				selectedTargetBone = -1;
				isPlayAnimation = false;
			}
			else
				selectedTargetBone = bone->Index();
		}

		for (UINT i = 0; i < bone->ChildCount(); i++)
		{
			BoneTreeCreator(model, bone->Child(i));
		}

		ImGui::TreePop();
	}
}

void TestAnimationTool::MainMenu()
{
	if (ImGui::BeginMenu("Open"))
	{
		if (ImGui::MenuItem("FBX Open"))
		{
			Path::OpenFileDialog
			(
				L""
				, L"FBX Files(*.fbx)\0*.fbx\0"
				, Assets
				, bind(&TestAnimationTool::OpenFbxFile, this, placeholders::_1)
			);
		}
		if (ImGui::MenuItem("Animation Data Open"))
		{
			Path::OpenFileDialog
			(
				L""
				, L"Animation Files(*.anim)\0*.anim\0"
				, Assets
				, bind(&TestAnimationTool::OpenAnimFile, this, placeholders::_1)
			);
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Save"))
	{
		ImGui::MenuItem("Animation...", NULL, &isSaveAnimDataWindowOpen);
		ImGui::EndMenu();
	}
}

void TestAnimationTool::ExportOption()
{
	if (attrDataFlag & (UINT)Fbx::FBX_INCLUDE_DATA::FBX_Model)
	{
		ImGui::CheckboxFlags("Model", &extDataFlag, (UINT)Fbx::FBX_INCLUDE_DATA::FBX_Model);
	}
	if (attrDataFlag & (UINT)Fbx::FBX_INCLUDE_DATA::FBX_Animation)
	{
		ImGui::CheckboxFlags("Animation", &extDataFlag, (UINT)Fbx::FBX_INCLUDE_DATA::FBX_Animation);
		ImGui::Separator();
		for (UINT i = 0; i < animationNames.size(); i++)
		{
			ImGui::Checkbox(animationNames[i].c_str(), &animExportIdxArray[i]);
		}
		ImGui::Separator();
	}

	if (ImGui::Button("Extract")) ExtractFbx(false);
	if (ImGui::Button("Cancel")) ExtractFbx(true);
}

void TestAnimationTool::ImguiHireacy()
{
	//Model
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Models"))
	{
		if (targetModel != nullptr)
		{
			ImGui::Text(String::ToString(L"Current Model: " + targetModel->GetName()).c_str());
			if (selectedTargetBone > -1)
			{
				ImGui::Text(String::ToString(L"Select Bone: " + targetModel->GetModel()->Bones()[selectedTargetBone]->Name()).c_str());
			}
		}

		for (UINT i = 0; i < models.size(); i++)
		{
			char str[10];
			string strName = String::ToString(models[i]->GetName());
			sprintf_s(str, "%d", i);
			strName.append("##model");
			strName.append(str);
			if (ImGui::TreeNode(strName.c_str()))
			{
#if true
				if (ImGui::IsItemClicked(1))
				{
					targetModel = models[i];
					selectedTargetBone = -1;
					selectedAnimationIdx = -1;
					selectedAnimationClipsIdx = -1;

					for (UINT i = 0; i < bonePin.size(); i++)
						SAFE_DELETE(bonePin[i]);

					bonePin.clear();

					for (UINT j = 0; j < models[i]->GetModel()->BoneCount(); j++)
					{
						ModelBone* b = models[i]->GetModel()->Bones()[j];
						DebugDraw* d = new DebugDraw(DebugDraw::DRAW_OBJECT_TYPE_SPHERE);
						d->Scale(1.5f, 1.5f, 1.5f);
						d->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
						bonePin.push_back(d);
					}
				}

				BoneTreeCreator(models[i], models[i]->GetModel()->BoneByIndex(0));
#else
				strName = "Select##";
				strName.append(str);
				if ((targetModel != models[i]) && ImGui::Button(strName.c_str()))
				{
					if (targetModel != nullptr)
					{
						targetModel->RemoveAllClip(false);
						targetModel->SettingDefaultPose();
					}
					targetModel = models[i];
					selectedTargetBone = -1;
					selectedAnimationIdx = -1;
					selectedAnimationClipsIdx = -1;

					for (UINT i = 0; i < bonePin.size(); i++)
						SAFE_DELETE(bonePin[i]);

					bonePin.clear();

					for (UINT j = 0; j < models[i]->GetModel()->BoneCount(); j++)
					{
						ModelBone* b = models[i]->GetModel()->Bones()[j];
						DebugDraw* d = new DebugDraw(DebugDraw::DRAW_OBJECT_TYPE_SPHERE);
						d->Scale(1.5f, 1.5f, 1.5f);
						d->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
						bonePin.push_back(d);
					}
				}
				if (targetModel == models[i])
				{
					vector<ModelBone *> bones = targetModel->GetModel()->Bones();
					for (UINT i = 0; i < targetModel->GetModel()->Bones().size(); i++)
					{
						if (ImGui::Button(String::ToString(targetModel->GetModel()->Bones()[i]->Name()).c_str()))
						{
							if (selectedTargetBone == i)
							{
								selectedTargetBone = -1;
								isPlayAnimation = false;
							}
							else
								selectedTargetBone = i;
						}
					}
				}
#endif
				ImGui::TreePop();
			}
		}//for (UINT i = 0; i < models.size(); i++)
	}//if (ImGui::CollapsingHeader("Models"))

	ImGui::Separator();
	ImGui::Separator();

	//Animation Clips
	if (ImGui::CollapsingHeader("Animations"))
	{
		isHiaracyAnimDown = true;
		for (UINT i = 0; i < clips.size(); i++)
		{
			char str[50];
			string buttonName = String::ToString(clips[i]->GetName());
			buttonName.append("##");
			sprintf_s(str, "%d", i);
			buttonName.append(str);
			if (ImGui::TreeNode(buttonName.c_str()))
			{
				if (targetModel != nullptr)
				{
					int idx = targetModel->ContainClip(clips[i]);
					if (idx == -1)
					{
						buttonName = "Insert to Model##anim";
						buttonName.append(str);
						if (ImGui::Button(buttonName.c_str()))
						{
							targetModel->AddClip(clips[i]);
							if (targetModel->GetClipCount() == 1)
								targetModel->Play(0, false, 0.0f, 20.0f);
						}
					}
					else
					{
						buttonName = "Remove to Model##anim";
						buttonName.append(str);
						if (ImGui::Button(buttonName.c_str()))
						{
							if (idx == selectedAnimationIdx)
							{
								targetModel->SettingDefaultPose();
								selectedAnimationIdx = -1;
								selectedAnimationClipsIdx = -1;
							}

							if ((targetModel->GetClipCount() - 1) < 1)
							{
								targetModel->SettingDefaultPose();
							}

							targetModel->DelClip(clips[i]);
						}

						if (selectedAnimationIdx != idx)
						{
							ImGui::SameLine();
							buttonName = "Select##anim";
							buttonName.append(str);
							if (ImGui::Button(buttonName.c_str()))
							{
								selectedAnimationIdx = idx;
								selectedAnimationClipsIdx = i;
								targetModel->Play(selectedAnimationIdx, false, (isPlayAnimation ? 20.0f : 0.0f), 20.0f);
							}
						}
					}
				}

				ImGui::TreePop();
			}

		}//for (UINT i = 0; i < clips.size(); i++)
	}//if (ImGui::CollapsingHeader("Animations"))
	else
	{
		isHiaracyAnimDown = false;
	}

}

void TestAnimationTool::ImguiInspector()
{
	if (targetModel != nullptr)
	{
		if (ImGui::CollapsingHeader("Model Information"))
		{
			ImGui::Separator();

			ID3D11ShaderResourceView* diffuseMap = nullptr;
			ID3D11ShaderResourceView* normalMap = nullptr;
			ID3D11ShaderResourceView* specularMap = nullptr;
			for (UINT i = 0; i < targetModel->GetModel()->MaterialCount(); i++)
			{
				Texture* t = nullptr;
				t = targetModel->GetModel()->MaterialByIndex(i)->GetDiffuseMap();
				if (t != nullptr) diffuseMap = t->GetView();
				t = targetModel->GetModel()->MaterialByIndex(i)->GetNormalMap();
				if (t != nullptr) normalMap = t->GetView();
				t = targetModel->GetModel()->MaterialByIndex(i)->GetSpecularMap();
				if (t != nullptr) specularMap = t->GetView();

				ImGui::Text("DiffuseMap"); ImGui::SameLine(0.0f, 21.0f); ImGui::Text("NormalMap"); ImGui::SameLine(0.0f, 29.0f); ImGui::Text("SpecularMap");
				if (ImGui::ImageButton(diffuseMap, ImVec2(75.0f, 75.0f)))
				{
					Path::OpenFileDialog
					(
						L""
						, L"PNG Files(*.png)\0 * .png\0"
						, Assets
						, bind(&Material::SetDiffuseMapA, targetModel->GetModel()->MaterialByIndex(i), placeholders::_1)
					);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton(normalMap, ImVec2(75.0f, 75.0f)))
				{
					Path::OpenFileDialog
					(
						L""
						, L"PNG Files(*.png)\0 * .png\0"
						, Assets
						, bind(&Material::SetNormalMapA, targetModel->GetModel()->MaterialByIndex(i), placeholders::_1)
					);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton(specularMap, ImVec2(75.0f, 75.0f)))
				{
					Path::OpenFileDialog
					(
						L""
						, L"PNG Files(*.png)\0 * .png\0"
						, Assets
						, bind(&Material::SetSpecularMapA, targetModel->GetModel()->MaterialByIndex(i), placeholders::_1)
					);
				}
				ImGui::Separator();
			}//for (targetModel->GetModel()->MaterialCount())

			ImGui::Separator();

			// Information inspector for selected bone.
			D3DXVECTOR3 S, R, T;

			S = targetModel->Scale();
			R = targetModel->Rotation();
			T = targetModel->Position();

			ImGui::SliderFloat3("Scale##Model", (float *)&S, 0.0001f, 10.0f);
			ImGui::SliderFloat3("Rotation##Model", (float *)&R, -Math::PI, Math::PI);
			ImGui::SliderFloat3("Position##Model", (float *)&T, -100.0f, 100.0f);

			targetModel->Scale(S);
			targetModel->Rotation(R);
			targetModel->Position(T);

		}//if (ImGui::CollapsingHeader("Model Information"))

		if (
			(selectedTargetBone > -1) &&
			ImGui::CollapsingHeader("Bone Information"))
		{
			ImGui::Separator();
			ImGui::Separator();

			// Information inspector for selected bone.
			string strS = "Scale##", strR = "Rotation##", strT = "Position##";
			char str[5];
			sprintf_s(str, "%d", selectedTargetBone);
			strS.append(str);
			strR.append(str);
			strT.append(str);

			ImGui::SliderFloat3(strS.c_str(), (float *)&m_S, 0.0001f, 10.0f);
			ImGui::SliderFloat3(strR.c_str(), (float *)&m_R, -Math::PI, Math::PI);
			ImGui::SliderFloat3(strT.c_str(), (float *)&m_T, -100.0f, 100.0f);

			D3DXMATRIX boneMatrix;
#if false
			D3DXMATRIX mS, mR, mT;

			D3DXMatrixScaling(&mS, m_S.x, m_S.y, m_S.z);

			D3DXQuaternionRotationYawPitchRoll(&m_qR, m_R.y, m_R.x, m_R.z);
			D3DXMatrixRotationQuaternion(&mR, &m_qR);

			D3DXMatrixTranslation(&mT, m_T.x, m_T.y, m_T.z);

			boneMatrix = mS * mR * mT;
#else
			CalcData(m_S, m_R, m_T, boneMatrix);
			targetModel->GetModel()->BoneByIndex(selectedTargetBone)->Local(boneMatrix);
			targetModel->Update();
#endif

		}//if (ImGui::CollapsingHeader("Bone Information"))

		if (
			((selectedTargetBone > -1) && (selectedAnimationClipsIdx > -1)) &&
			ImGui::CollapsingHeader("Animation Information")
			)
		{
			ImGui::Separator();
			ImGui::Separator();

			vector<ModelKeyframe::Transform> vec;
			clips[selectedAnimationClipsIdx]->GetKeyframeTransform(targetModel->GetModel()->BoneByIndex(selectedTargetBone), vec);

			for (ModelKeyframe::Transform tf : vec)
			{
				ImGui::Text("Time: %.3f -::- S(%.2f, %.2f, %.2f), "
					, tf.Time
					, tf.S.x, tf.S.y, tf.S.z);
				ImGui::Text("R(%.2f, %.2f, %.2f, %.2f), T(%.2f, %.2f, %.2f)"
					, tf.R.x, tf.R.y, tf.R.z, tf.R.w
					, tf.T.x, tf.T.y, tf.T.z);
				ImGui::Separator();
			}
		}
	}//if (targetModel != nullptr)

	if (isHiaracyAnimDown && selectedAnimationClipsIdx > -1)
	{
		if (ImGui::CollapsingHeader("Animation Bone List"))
		{
			clips[selectedAnimationClipsIdx]->EditAnimBoneName();
		}
	}
}

void TestAnimationTool::ImguiTimeSpector()
{
	ImGui::SliderFloat("Speed", &animationPlaySpeed, 1.0f, 5000.0f);
	ImGui::SliderFloat("Animation Progress", &animationTime, 0.0f, targetModel->GetClipDuration(selectedAnimationIdx));
	ImGui::Checkbox("Repeat", &isRepeat);
	if (!isPlayAnimation && ImGui::Button("Play Anim")) isPlayAnimation = true;
	else if (isPlayAnimation && ImGui::Button("Stop Anim")) isPlayAnimation = false;

	if (ImGui::Button("Insert Pose"))
	{
#if false
		targetModel->AddCurrentMotion(selectedAnimationIdx, animationTime);
#else
		for (ModelBone* bone : targetModel->GetModel()->Bones())
		{
			ModelKeyframe::Transform tf;
			D3DXMATRIX boneMatrix;
			D3DXMATRIX _matS, _matR, _matT;
			D3DXVECTOR3 _S, _R, _T;
			D3DXQUATERNION _qR;

			if (bone->Index() == selectedTargetBone)
			{
				D3DXMatrixScaling(&_matS, m_S.x, m_S.y, m_S.z);
				D3DXMatrixRotationYawPitchRoll(&_matR, m_R.y, m_R.x, m_R.z);
				D3DXMatrixTranslation(&_matT, m_T.x, m_T.y, m_T.z);
				boneMatrix = _matS * _matR * _matT;
			}
			else
			{
				boneMatrix =
					clips[selectedAnimationClipsIdx]->GetKeyframeOriginMatrix(
						targetModel->GetModel()->BoneByIndex(bone->Index())
						, animationTime
						, false
					);
			}

			tf.Time = animationTime;
			D3DXMatrixDecompose(&tf.S, &tf.R, &tf.T, &boneMatrix);

			clips[selectedAnimationClipsIdx]->AddMotion(bone, tf);
		}

		ModelKeyframe::Transform dummy;
		clips[selectedAnimationClipsIdx]->AddMotion(NULL, dummy, true);
#endif
	}

	ImGui::SameLine();
	
	if (ImGui::Button("Delete Pose"))
	{
	}
}

