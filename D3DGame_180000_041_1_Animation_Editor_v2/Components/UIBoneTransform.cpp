#include "stdafx.h"
#include "UIBoneTransform.h"

#include "../Model/ModelClip.h"
#include "./Objects/GameModel.h"

UIBoneTransform::UIBoneTransform()
	: targetModel(nullptr), targetBone(nullptr), targetClip(nullptr)
{
	this->uiType = ComponentUI::UIType::UIBoneTransform;
	radioMatrix = 0;
}

UIBoneTransform::~UIBoneTransform()
{
}

void UIBoneTransform::Update()
{
	//if (targetBone == nullptr || containUIName.length() < 1) return;
}

void UIBoneTransform::Render()
{
	if (this->targetModel == nullptr || this->targetBone == nullptr || containUIName.length() < 1) return;

	ImGui::Begin(containUIName.c_str());
	{
		ImGui::Text("Transform :: Bone");

		ImGui::BeginGroup();
		{
			ImGui::RadioButton("Global", &radioMatrix, 0);
			ImGui::RadioButton("Local", &radioMatrix, 1);
		}
		ImGui::EndGroup();

		if (targetClip != nullptr)
		{
			ExportTransform();
			{
				ImGui::SliderFloat3(strS.c_str(), (float *)&vS, 0.0001f, 100.0f);
				ImGui::SliderFloat3(strR.c_str(), (float *)&vR, -Math::PI, Math::PI);
				//ImGui::SliderFloat3(strT.c_str(), (float *)&vT, -1000.0f, 1000.0f);
				ImGui::DragFloat3(strT.c_str(), (float *)&vT);
			}
			CalcTranform();

			targetModel->Update();
		}
		else
		{
			D3DXMATRIX boneTransform;

			if (radioMatrix == 0)
				boneTransform = targetBone->Global();
			else if (radioMatrix == 1)
				boneTransform = targetBone->Local();

			D3DXMatrixDecompose(&vS, &qR, &vT, &boneTransform);
			Math::toEulerAngle(qR, vR);

			ImGui::SliderFloat3(strS.c_str(), (float *)&vS, 0.0001f, 10.0f);
			ImGui::SliderFloat3(strR.c_str(), (float *)&vR, -Math::PI, Math::PI);
			ImGui::DragFloat3(strT.c_str(), (float *)&vT);

			D3DXMATRIX S, R, F;
			D3DXMatrixScaling(&S, vS.x, vS.y, vS.z);
			D3DXMatrixRotationYawPitchRoll(&R, vR.y, vR.x, vR.z);
			F = S * R;
			F._41 = vT.x; F._42 = vT.y; F._43 = vT.z;

			if (radioMatrix == 0)
				targetBone->Global(F);
			else if (radioMatrix == 1)
				targetBone->Local(F);
		}

		ImGui::Separator();
	}
	ImGui::End();
}

void UIBoneTransform::ChangeTarget(ModelBone * targetBone, GameModel * targetModel, class ModelClip * targetClip)
{
	this->targetBone = targetBone;
	this->targetModel = targetModel;
	this->targetClip = targetClip;

	if (this->targetBone != nullptr)
	{
		strS = "Scale##BT" + to_string(this->targetBone->Index());
		strR = "Rotation##BT" + to_string(this->targetBone->Index());
		strT = "Position##BT" + to_string(this->targetBone->Index());
	}
}

void UIBoneTransform::ExportTransform()
{
	D3DXMATRIX boneMatrix;

	if (targetClip == nullptr) return;

	boneMatrix =
		targetClip->GetKeyframeOriginMatrix(
			targetBone
			, targetClip->GetPlayTime()
			, false
		);

	D3DXMatrixDecompose(&vS, &qR, &vT, &boneMatrix);
	Math::toEulerAngle(qR, vR);
}

void UIBoneTransform::CalcTranform()
{
	D3DXMATRIX boneMatrix;

	D3DXMATRIX _matS, _matR;
	D3DXMatrixScaling(&_matS, vS.x, vS.y, vS.z);
	D3DXMatrixRotationYawPitchRoll(&_matR, vR.y, vR.x, vR.z);

	boneMatrix = _matS * _matR;
	boneMatrix._41 = vT.x; boneMatrix._42 = vT.y; boneMatrix._43 = vT.z;

	D3DXMATRIX invGlobal, parent;
	invGlobal = targetBone->Global();
	D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

	int parentIndex = targetBone->ParentIndex();
	if (parentIndex < 0)
	{
		parent = boneMatrix;
	}
	else
	{
		parent = boneMatrix * targetBone->Parent()->Global();
	}

	localMatrix = invGlobal * parent;
}
