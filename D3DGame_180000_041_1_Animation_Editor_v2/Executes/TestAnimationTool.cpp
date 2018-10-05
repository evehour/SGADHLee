#include "stdafx.h"
#include "TestAnimationTool.h"
#include "../Objects/GameAnimModel.h"

#include "../Fbx/Exporter.h"
#include "../Model/ModelClip.h"
#include "../Objects/DebugDraw.h"

#include "../Components/AssetManager.h"

#include "../Components/UIAnimationInformation.h"
#include "../Components/UIBoneList.h"
#include "../Components/UIBoneTransform.h"
#include "../Components/UIMaterial.h"
#include "../Components/UIModelTransform.h"

TestAnimationTool::TestAnimationTool(ExecuteValues * values)
	: Execute(values)
	, targetModel(nullptr)
	, animExportIdxArray(nullptr)
	, bOpenFbxAttr(false), bFileLoadComplete(false)
	, attrDataFlag(0), extDataFlag(0)
	, selectedTargetBone(-1), selectedAnimationClipsIdx(-1), selectedAnimationIdx(-1)
	, animationTime(0.0f), isRepeat(false), isPlayAnimation(false), animationPlaySpeed(20.0f)
	, isSaveAnimDataWindowOpen(false), isSaveMeshDataWindowOpen(false)
	, m_S(1.0f, 1.0f, 1.0f), m_R(0.0f, 0.0f, 0.0f), m_qR(0.0f, 0.0f, 0.0f, 0.0f), m_T(0.0f, 0.0f, 0.0f)
{
	assetManager = new AssetManager();

	depthStencilState[0] = new DepthStencilState();
	depthStencilState[1] = new DepthStencilState();
	depthStencilState[1]->DepthEnable(false);

	uicomponents.push_back(new UIBoneList());
	uicomponents.push_back(new UIBoneTransform());
	uicomponents.push_back(new UIMaterial());
	uicomponents.push_back(new UIModelTransform());
	uicomponents.push_back(new UIAnimationInformation());

	uicomponents[(UINT)ComponentUI::UIType::UIBoneList]->ChangeContainUIName("Hireacy");
	uicomponents[(UINT)ComponentUI::UIType::UIBoneTransform]->ChangeContainUIName("Inspector");
	uicomponents[(UINT)ComponentUI::UIType::UIMaterial]->ChangeContainUIName("Inspector");
	uicomponents[(UINT)ComponentUI::UIType::UIModelTransform]->ChangeContainUIName("Inspector");
	uicomponents[(UINT)ComponentUI::UIType::UIAnimationInformation]->ChangeContainUIName("Inspector");
}

TestAnimationTool::~TestAnimationTool()
{
	SAFE_DELETE(assetManager);
	
	for (UINT i = 0; i < bonePin.size(); i++)
		SAFE_DELETE(bonePin[i]);

	for (UINT i = 0; i < uicomponents.size(); i++)
		SAFE_DELETE(uicomponents[i]);
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
					((UIBoneList*)uicomponents[(UINT)ComponentUI::UIType::UIBoneList])->SetSelectedTargetBone(selectedTargetBone);
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

	// UIComponent
	{
		ModelClip* _uiClip = nullptr;
		ModelBone* _uiBone = nullptr;
		
		if (selectedAnimationClipsIdx > -1)
			_uiClip = assetManager->GetClip(selectedAnimationClipsIdx);
		if (selectedTargetBone > -1)
			_uiBone = targetModel->GetModel()->BoneByIndex(selectedTargetBone);

		UINT _comSize = uicomponents.size();
		for (UINT i = 0; i < _comSize; i++)
		{
			switch (uicomponents[i]->GetUIType())
			{
			case ComponentUI::UIType::UIAnimationInformation:
				{
					UIAnimationInformation* uianim = reinterpret_cast<UIAnimationInformation*>(uicomponents[i]);
					uianim->ChangeTarget(_uiClip, targetModel);
				}
				break;
			case ComponentUI::UIType::UIBoneList:
				{
					UIBoneList* uibonelist = reinterpret_cast<UIBoneList*>(uicomponents[i]);
					uibonelist->ChangeTarget(targetModel);
				}
				break;
			case ComponentUI::UIType::UIBoneTransform:
				{
					UIBoneTransform* uibonetf = reinterpret_cast<UIBoneTransform*>(uicomponents[i]);
					uibonetf->ChangeTarget(_uiBone, targetModel, _uiClip);
				}
				break;
			case ComponentUI::UIType::UIMaterial:
				{
					UIMaterial* uimaterial = reinterpret_cast<UIMaterial*>(uicomponents[i]);
					uimaterial->ChangeTarget(targetModel);
				}
				break;
			case ComponentUI::UIType::UIModelTransform:
				{
					UIModelTransform* uimodeltf = reinterpret_cast<UIModelTransform*>(uicomponents[i]);
					uimodeltf->ChangeTarget(targetModel);
				}
				break;
			}

			uicomponents[i]->Update();
		}
	}
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

	ImGuiRender();
}

void TestAnimationTool::PostRender()
{
	if (selectedTargetBone > -1)
	{
		// 방법 1
		//{
		//	D3DXMATRIX WVP = boneWorld * matVP;
		//
		//	D3DXVECTOR4 worldToScreen(0, 0, 0, 1);
		//	D3DXVec4Transform(&worldToScreen, &worldToScreen, &WVP);
		//
		//	// NDC 공간으로 변환
		//	float wScreenX = worldToScreen.x / worldToScreen.w;
		//	float wScreenY = worldToScreen.y / worldToScreen.w;
		//	float wScreenZ = worldToScreen.z / worldToScreen.w;
		//
		//	// -1~1 -> 0~1
		//	float nScreenX = (wScreenX + 1) * 0.5f;
		//	float nScreenY = 1.0f - (wScreenY + 1) * 0.5f;
		//
		//	// 최종
		//	resultX = nScreenX * screenSize.x;
		//	resultY = nScreenY * screenSize.y;
		//}

		D3DDesc desc;
		D3D::GetDesc(&desc);

		D3DXMATRIX matView, matProj, matVP, matDelta;
		values->MainCamera->Matrix(&matView);
		values->Perspective->GetMatrix(&matProj);
		matVP = matView * matProj;

		D3DXVECTOR3 pos = bonePin[selectedTargetBone]->Position();
		D3DXVec3TransformCoord(&pos, &pos, &matVP);

		pos.x = desc.Width * (pos.x + 1.0f) * 0.5f;
		pos.y = desc.Height * (1.0f - (pos.y + 1.0f) * 0.5f);

		RECT rect = { (LONG)pos.x, (LONG)pos.y, (LONG)pos.x + 100, (LONG)pos.y + 100 };
		DirectWrite::Get()->RenderText(
			targetModel->GetModel()->BoneByIndex(selectedTargetBone)->Name()
			, rect
			, 15
			, L"돋움체"
			, D3DXCOLOR(1, 1, 0, 1));
	}
}

void TestAnimationTool::ImGuiRender()
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
	else if (isSaveMeshDataWindowOpen)
		ImGui::OpenPopup("MeshSave");

	if (ImGui::BeginPopupModal("AnimationSave"))
	{
		UINT size = assetManager->GetClipCount();
		if (size > 0)
		{
			char** _anims;
			_anims = new char*[size];
			for (UINT i = 0; i < size; i++)
			{
				string _name = String::ToString(assetManager->GetClip(i)->GetName());
				_anims[i] = new char[_name.length() + 1];
				strcpy_s(_anims[i], _name.length() + 1, _name.c_str());
			}

			ImGui::ListBox("Animation\nList", &saveAnimListIdx, _anims, size);

			if (ImGui::Button("Save"))
			{
				Path::SaveFileDialog
				(
					L""
					, L"Animation Files(*.anim)\0*.anim\0"
					, Assets
					, bind(&ModelClip::WriteClip, assetManager->GetClip(saveAnimListIdx), placeholders::_1)
				);
			}

			ImGui::SameLine(0.0f, 28.0f);

			if (ImGui::Button("Close"))
			{
				isSaveAnimDataWindowOpen = false;
				ImGui::CloseCurrentPopup();
			}

			for (UINT i = 0; i < size; i++)
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
	else if (ImGui::BeginPopupModal("MeshSave"))
	{
		UINT size = assetManager->GetModelCount();
		if (size > 0)
		{
			char** _models;
			_models = new char*[size];
			for (UINT i = 0; i < size; i++)
			{
				string _name = String::ToString(assetManager->GetModel(i)->GetName());
				_models[i] = new char[_name.length() + 1];
				strcpy_s(_models[i], _name.length() + 1, _name.c_str());
			}

			ImGui::ListBox("Model\nList", &saveModelListIdx, _models, size);

			if (ImGui::Button("Save"))
			{
				Path::SaveFileDialog
				(
					L""
					, L"Mesh Files(*.mesh)\0*.mesh\0"
					, Assets
					, bind(&Model::WriteModel, assetManager->GetModel(saveModelListIdx)->GetModel(), placeholders::_1)
				);
			}

			ImGui::SameLine(0.0f, 28.0f);

			if (ImGui::Button("Close"))
			{
				isSaveMeshDataWindowOpen = false;
				ImGui::CloseCurrentPopup();
			}

			for (UINT i = 0; i < size; i++)
				SAFE_DELETE(_models[i]);
			SAFE_DELETE_ARRAY(_models);
			ImGui::TextWrapped("                                               ");
		}
		else
		{
			isSaveMeshDataWindowOpen = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Pre-Extract process
	if (bOpenFbxAttr)
	{
		ImGui::Begin("Export Option");
		{
			ExportOption();
		}
		ImGui::End();
	}

	// Post-Load
	if (bFileLoadComplete)
	{
		ImGui::Begin("Hireacy");
		{
			// Fixed window position
			{
				//ImGui::SetWindowSize(ImVec2(200, 668));
				//ImGui::SetWindowPos(ImVec2(16, 34));
			}
			ImguiHierarchy();

		}//ImGui::Begin("Hireacy");
		ImGui::End();

		ImGui::Begin("Inspector");
		{
			// Fixed window position
			{
				//ImGui::SetWindowPos(ImVec2(936, 34));
				//ImGui::SetWindowSize(ImVec2(500, 668));

				uicomponents[(UINT)ComponentUI::UIType::UIModelTransform]->Render();
				uicomponents[(UINT)ComponentUI::UIType::UIMaterial]->Render();
				uicomponents[(UINT)ComponentUI::UIType::UIBoneTransform]->Render();
				uicomponents[(UINT)ComponentUI::UIType::UIAnimationInformation]->Render();
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

void TestAnimationTool::OpenModelFile(wstring fName)
{
	GameAnimModel* _model = nullptr;
	wstring _matFolder, _matFile, _meshFolder, _meshFile, _fileName;
	_matFolder = _meshFolder = Path::GetDirectoryName(fName);
	_fileName = Path::GetFileNameWithoutExtension(fName);
	_matFile = _fileName + L".material";
	_meshFile = _fileName + L".mesh";

	_model = new GameAnimModel(_matFolder, _matFile, _meshFolder, _meshFile);
	_model->SetName(_fileName);

	assetManager->AddModel(_model);

	bFileLoadComplete = true;
}

void TestAnimationTool::OpenAnimFile(wstring fName)
{
	ModelClip* clip = nullptr;
	wstring _name = Path::GetFileNameWithoutExtension(fName);
	clip = new ModelClip(fName);
	clip->SetName(_name);
	assetManager->AddClip(clip);

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
		animModel->SetName(fileName);
		assetManager->AddModel(animModel);
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
				assetManager->AddClip(clip);
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

	if (selectedAnimationClipsIdx < 0) return;
	if ((int)assetManager->GetClipCount() <= selectedAnimationClipsIdx) return;

	boneMatrix =
		assetManager->GetClip(selectedAnimationClipsIdx)->GetKeyframeOriginMatrix(
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
	//if(ImGui::TreeNodeEx(String::ToString(bone->Name()).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("STMB", (void*)bone, sizeof(ModelBone));
			ImGui::Text("%s", String::ToString(bone->Name()).c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("STMB");
			if (payload != NULL)
			{
				IM_ASSERT(payload->DataSize == sizeof(ModelBone));
				ModelBone* data = (ModelBone*)payload->Data;
				if (data->Index() != bone->Index())
				{
					ModelBone* pBone = model->GetModel()->BoneByIndex(data->Index());
					pBone->SetParent(bone);
					ImGui::EndDragDropTarget();
					ImGui::TreePop();
					return;
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (selectedTargetBone == bone->Index())
		{
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 0, 255, 128)
			);
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
		if (ImGui::MenuItem("Model Open"))
		{
			Path::OpenFileDialog
			(
				L""
				, L"mesh Files(*.mesh)\0*.mesh\0"
				, Assets
				, bind(&TestAnimationTool::OpenModelFile, this, placeholders::_1)
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
		ImGui::MenuItem("Model...", NULL, &isSaveMeshDataWindowOpen);
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Add Bone"))
		{
			targetModel->AddBone();

			Model* _m = targetModel->GetModel();

			for (UINT i = 0; i < bonePin.size(); i++)
				SAFE_DELETE(bonePin[i]);

			bonePin.clear();

			for (UINT j = 0; j < _m->BoneCount(); j++)
			{
				ModelBone* b = _m->Bones()[j];
				DebugDraw* d = new DebugDraw(DebugDraw::DRAW_OBJECT_TYPE_SPHERE);
				d->Scale(1.5f, 1.5f, 1.5f);
				d->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				bonePin.push_back(d);
			}
		}
		if (ImGui::MenuItem("Delete Bone"))
		{
			;
		}
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

void TestAnimationTool::ImguiHierarchy()
{
	//Model
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Models"))
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Right click is select.");
		if (targetModel != nullptr)
		{
			ImGui::Text(String::ToString(L"Current Model: " + targetModel->GetName()).c_str());
			if (selectedTargetBone > -1)
			{
				ImGui::Text(String::ToString(L"Select Bone: " + targetModel->GetModel()->Bones()[selectedTargetBone]->Name()).c_str());
			}
		}

		UINT _size = assetManager->GetModelCount();
		for (UINT i = 0; i < _size; i++)
		{
			char str[10];
			string strName;
			GameAnimModel* _assetModel = assetManager->GetModel(i);
			strName = String::ToString(_assetModel->GetName());
			//strName = strName + "##md" + to_string(i);
			strName = to_string(i);

			Model* _model = _assetModel->GetModel();
			if (ImGui::TreeNode(strName.c_str()))
			{
				if (ImGui::IsItemClicked(1))
				{
					targetModel = _assetModel;
					selectedTargetBone = -1;
					selectedAnimationIdx = -1;
					selectedAnimationClipsIdx = -1;

					for (UINT i = 0; i < bonePin.size(); i++)
						SAFE_DELETE(bonePin[i]);

					bonePin.clear();

					for (UINT j = 0; j < _model->BoneCount(); j++)
					{
						ModelBone* b = _model->Bones()[j];
						DebugDraw* d = new DebugDraw(DebugDraw::DRAW_OBJECT_TYPE_SPHERE);
						d->Scale(1.5f, 1.5f, 1.5f);
						d->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
						bonePin.push_back(d);
					}
				}

				UIBoneList* _uiBoneList = reinterpret_cast<UIBoneList *>(uicomponents[(UINT)ComponentUI::UIType::UIBoneList]);
				_uiBoneList->ChangeTarget(_assetModel);
				_uiBoneList->Render();
				selectedTargetBone = _uiBoneList->GetSelectedTargetBone();

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
		UINT _size = assetManager->GetClipCount();
		for (UINT i = 0; i < _size; i++)
		{
			char str[50];
			string buttonName;
			ModelClip* _assetClip = assetManager->GetClip(i);
			buttonName = String::ToString(_assetClip->GetName());
			buttonName.append("##");
			sprintf_s(str, "%d", i);
			buttonName.append(str);

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Right click is changing name to this animation.");
			if (ImGui::TreeNode(buttonName.c_str()))
			{
				if (ImGui::IsItemClicked(1))
				{
					sprintf_s(_changeAnimationName, "%s", String::ToString(_assetClip->GetName()).c_str());
					ImGui::OpenPopup("AnimNChg");
				}

				if (ImGui::BeginPopupModal("AnimNChg"))
				{
					ImGui::InputText("Animation name", _changeAnimationName, 1024);

					if (ImGui::Button("Confirm"))
					{
						_assetClip->SetName(String::ToWString(_changeAnimationName));
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					if (ImGui::Button("Close"))
					{
						ImGui::CloseCurrentPopup();
					}
					ImGui::TextWrapped("                                               ");
					ImGui::EndPopup();
				}

				if (targetModel != nullptr)
				{
					int idx = targetModel->ContainClip(_assetClip);
					if (idx == -1)
					{
						buttonName = "Insert to Model##anim";
						buttonName.append(str);
						if (ImGui::Button(buttonName.c_str()))
						{
							ModelClip* _fromAssetClip;
							//assetManager->CopyFromClips(i, &_fromAssetClip);
							_fromAssetClip = assetManager->GetClip(i);
							targetModel->AddClip(_fromAssetClip);
							if (targetModel->GetClipCount() == 1)
								targetModel->Play((UINT)0, false, 0.0f, 20.0f);
						}
					}
					else
					{
						buttonName = "Remove from Model##anim";
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

							targetModel->DelClip(_assetClip);
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
		ModelClip* _clip = assetManager->GetClip(selectedAnimationClipsIdx);

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
					_clip->GetKeyframeOriginMatrix(
						targetModel->GetModel()->BoneByIndex(bone->Index())
						, animationTime
						, false
					);
			}

			tf.Time = animationTime;
			D3DXMatrixDecompose(&tf.S, &tf.R, &tf.T, &boneMatrix);

			_clip->AddMotion(bone, tf);
		}

		ModelKeyframe::Transform dummy;
		_clip->AddMotion(NULL, dummy, true);
#endif
	}

	ImGui::SameLine();
	
	if (ImGui::Button("Delete Pose"))
	{
	}
}

