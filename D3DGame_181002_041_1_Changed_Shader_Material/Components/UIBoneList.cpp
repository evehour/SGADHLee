#include "stdafx.h"
#include "UIBoneList.h"
#include "./Objects/GameModel.h"
#include "UIBoneList.h"

UIBoneList::UIBoneList()
	: targetObject(nullptr), containUIName("")
{
}

UIBoneList::~UIBoneList()
{
}

void UIBoneList::Update()
{
}

void UIBoneList::PostRender()
{
	if (targetObject == nullptr || containUIName.length() < 1) return;
}

void UIBoneList::ChangeTarget(GameModel * target)
{
}

void UIBoneList::ChangeContainUIName(string containUIName)
{
}

int UIBoneList::BoneTreeCreator(GameModel* model, ModelBone* bone)
{
	int selectedTargetBone = -1;

	if (ImGui::TreeNode(String::ToString(bone->Name()).c_str()))
	//if(ImGui::TreeNodeEx(String::ToString(bone->Name()).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::IsItemClicked(1))
		{
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

