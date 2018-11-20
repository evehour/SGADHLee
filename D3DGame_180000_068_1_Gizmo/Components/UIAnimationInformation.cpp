#include "stdafx.h"
#include "UIAnimationInformation.h"
#include "../Model/ModelClip.h"
#include "../Objects/GameModel.h"

UIAnimationInformation::UIAnimationInformation()
	: totalFrame(0)
{
	this->uiType = ComponentUI::UIType::UIAnimationInformation;
}

UIAnimationInformation::~UIAnimationInformation()
{
}

void UIAnimationInformation::Update()
{
}

void UIAnimationInformation::Render()
{
	if (targetObject == nullptr || targetClip == nullptr)
		return;

	ImGui::Begin(containUIName.c_str());
	{
		//if (ImGui::CollapsingHeader("Animation Information"))
		ImGui::Text("Animation Information");
		{
			ImGui::Text("Animation: %s", clipName.c_str());
			ImGui::Text("Duration: %.02f sec, Total Frame: %03d", targetClip->GetDuration(), totalFrame);
			ImGui::Separator();

			if (ImGui::TreeNode("Time Table"))
			{
				vector<ModelKeyframe::Transform> vec;
				targetClip->GetKeyframeTransform(targetObject->GetModel()->BoneByIndex(0), vec);
				UINT i = 0;
				for (ModelKeyframe::Transform tf : vec)
				{
					ImGui::Text("Frame: %03d - Time: %.3f"
						, i
						, tf.Time);
					ImGui::Separator();
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Animation target bones"))
			{
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Drag and drop bone from Hierarchy or Right click");
				targetClip->EditAnimBoneName();
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}

void UIAnimationInformation::ChangeTarget(ModelClip * targetClip, GameModel * targetModel)
{
	this->targetClip = targetClip;
	this->targetObject = targetModel;

	if (this->targetClip != nullptr)
	{
		this->clipName = String::ToString(this->targetClip->GetName());

		vector<ModelKeyframe::Transform> vec;
		this->targetClip->GetKeyframeTransform(
			this->targetObject->GetModel()->BoneByIndex(0)
			, vec
		);

		totalFrame = 0;
		for (ModelKeyframe::Transform tf : vec)
		{
			totalFrame++;
		}
	}
}
