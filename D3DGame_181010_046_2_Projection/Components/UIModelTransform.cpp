#include "stdafx.h"
#include "UIModelTransform.h"

#include "./Objects/GameModel.h"

UIModelTransform::UIModelTransform()
	: targetObject(nullptr)
{
	this->uiType = ComponentUI::UIType::UIModelTransform;
}

UIModelTransform::~UIModelTransform()
{
}

void UIModelTransform::Update()
{
	if (targetObject == nullptr || containUIName.length() < 1) return;

	vS = targetObject->Scale();
	vR = targetObject->Rotation();
	vT = targetObject->Position();
}

void UIModelTransform::Render()
{
	if (targetObject == nullptr || containUIName.length() < 1) return;

	ImGui::Begin(containUIName.c_str());
	{
		//if (ImGui::CollapsingHeader("Transform :: Model"))
		ImGui::Text("Transform :: Model");
		{
			ImGui::SliderFloat3("Scale##UITr", (float *)&vS, 0.0001f, 100.0f);
			ImGui::SliderFloat3("Rotation##UITr", (float *)&vR, -Math::PI, Math::PI);
			ImGui::SliderFloat3("Position##UITr", (float *)&vT, -1000.0f, 1000.0f);

			targetObject->Scale(vS);
			targetObject->Rotation(vR);
			targetObject->Position(vT);
		}

		ImGui::Separator();
	}
	ImGui::End();
}

void UIModelTransform::ChangeTarget(GameModel * target)
{
	targetObject = target;
}
