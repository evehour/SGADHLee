#include "stdafx.h"
#include "UIMaterial.h"
#include "./Objects/GameModel.h"

UIMaterial::UIMaterial()
	: targetObject(nullptr), containUIName("")
{
}

UIMaterial::~UIMaterial()
{
}

void UIMaterial::Update()
{
}

void UIMaterial::PostRender()
{
	if (targetObject == nullptr || containUIName.length() < 1) return;

	if (ImGui::CollapsingHeader("Material"))
	{
		for (UINT i = 0; i < materialCount; i++)
		{
			ID3D11ShaderResourceView* diffuseMap = nullptr;
			ID3D11ShaderResourceView* normalMap = nullptr;
			ID3D11ShaderResourceView* specularMap = nullptr;
			Texture* t = nullptr;

			t = targetObject->GetModel()->MaterialByIndex(i)->GetDiffuseMap();
			if (t != nullptr) diffuseMap = t->GetView();
			t = targetObject->GetModel()->MaterialByIndex(i)->GetNormalMap();
			if (t != nullptr) normalMap = t->GetView();
			t = targetObject->GetModel()->MaterialByIndex(i)->GetSpecularMap();
			if (t != nullptr) specularMap = t->GetView();

			ImGui::Text("DiffuseMap"); ImGui::SameLine(0.0f, 21.0f); ImGui::Text("NormalMap"); ImGui::SameLine(0.0f, 29.0f); ImGui::Text("SpecularMap");
			if (ImGui::ImageButton(diffuseMap, ImVec2(75.0f, 75.0f)))
			{
				Path::OpenFileDialog
				(
					L""
					, L"PNG Files(*.png)\0 * .png\0"
					, Assets
					, bind(&Material::SetDiffuseMapA, targetObject->GetModel()->MaterialByIndex(i), placeholders::_1)
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
					, bind(&Material::SetNormalMapA, targetObject->GetModel()->MaterialByIndex(i), placeholders::_1)
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
					, bind(&Material::SetSpecularMapA, targetObject->GetModel()->MaterialByIndex(i), placeholders::_1)
				);
			}

			if(i < (materialCount - 1))
				ImGui::Separator();
		}//if (materialCount)
	}//if (ImGui::CollapsingHeader)
}

void UIMaterial::ChangeTarget(GameModel * target)
{
	targetObject = target;
	materialCount = targetObject->GetModel()->MaterialCount();
}

void UIMaterial::ChangeContainUIName(string containUIName)
{
	this->containUIName = containUIName;
}
