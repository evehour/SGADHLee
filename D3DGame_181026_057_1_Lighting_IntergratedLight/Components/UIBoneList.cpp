#include "stdafx.h"
#include "UIBoneList.h"
#include "./Objects/GameModel.h"
#include "UIBoneList.h"

UIBoneList::UIBoneList()
	: targetObject(nullptr)
	, selectedTargetBone(-1)
{
	this->uiType = ComponentUI::UIType::UIBoneList;
}

UIBoneList::~UIBoneList()
{
}

void UIBoneList::Render()
{
	if (targetObject == nullptr) return;

	vector<ModelBone*> vec = targetObject->GetModel()->Bones();
	auto it = vec.begin();

	while (it != vec.end())
	{
		if ((*it)->Parent() == NULL)
		{
			BoneTreeCreator(targetObject, (*it));
		}
		it++;
	}
}

void UIBoneList::ChangeTarget(GameModel * target)
{
	this->targetObject = target;
}

int UIBoneList::BoneTreeCreator(GameModel* model, ModelBone* bone)
{
	if (ImGui::TreeNode(String::ToString(bone->Name()).c_str()))
	//if(ImGui::TreeNodeEx(String::ToString(bone->Name()).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::IsItemClicked(1))
		{
			selectedTargetBone = bone->Index();
		}

		if (ImGui::IsKeyDown(VK_TAB))
		{
			if (selectedTargetBone > -1)
			{
				ImGui::OpenPopup("BoneNChg");
				changeTargetBone = model->GetModel()->BoneByIndex(selectedTargetBone);
				sprintf_s(changeBoneName, "%s", String::ToString(changeTargetBone->Name()).c_str());
			}
		}

		if (ImGui::BeginPopupModal("BoneNChg"))
		{
			ImGui::InputText("Bone name", changeBoneName, 1024);

			if (ImGui::Button("Confirm"))
			{
				changeTargetBone->Name(String::ToWString(changeBoneName));
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
					return selectedTargetBone;
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
			int tmp = BoneTreeCreator(model, bone->Child(i));
			if (tmp > -1)
				selectedTargetBone = tmp;
		}

		ImGui::TreePop();
	}

	return selectedTargetBone;
}

