#include "stdafx.h"
#include "UIBoneTransform.h"

#include "../Model/ModelClip.h"
#include "./Objects/GameModel.h"

UIBoneTransform::UIBoneTransform()
	: targetModel(nullptr), targetBone(nullptr), targetClip(nullptr), containUIName("")
{
}

UIBoneTransform::~UIBoneTransform()
{
}

void UIBoneTransform::Update()
{
	//if (targetBone == nullptr || containUIName.length() < 1) return;
}

void UIBoneTransform::PostRender()
{
	if (this->targetModel == nullptr || this->targetBone == nullptr || containUIName.length() < 1) return;

	ImGui::Begin(containUIName.c_str());
	{
		if (targetClip != nullptr)
		{
			ExportTransform();
			if (ImGui::CollapsingHeader("Transform :: Bone"))
			{
				ImGui::SliderFloat3(strS.c_str(), (float *)&vS, 0.0001f, 10.0f);
				ImGui::SliderFloat3(strR.c_str(), (float *)&vR, -Math::PI, Math::PI);
				ImGui::SliderFloat3(strT.c_str(), (float *)&vT, -100.0f, 100.0f);
			}
			CalcTranform();

			targetModel->Update();
		}
		else
		{
			D3DXMATRIX boneTransform = targetBone->Local();
			D3DXMatrixDecompose(&vS, &qR, &vT, &boneTransform);
			Math::toEulerAngle(qR, vR);

			ImGui::SliderFloat3(strS.c_str(), (float *)&vS, 0.0001f, 10.0f);
			ImGui::SliderFloat3(strR.c_str(), (float *)&vR, -Math::PI, Math::PI);
			ImGui::SliderFloat3(strT.c_str(), (float *)&vT, -100.0f, 100.0f);

			D3DXMATRIX S, R, F;
			D3DXMatrixScaling(&S, vS.x, vS.y, vS.z);
			D3DXMatrixRotationYawPitchRoll(&R, vR.y, vR.x, vR.z);
			F = S * R;
			F._41 = vT.x; F._42 = vT.y; F._43 = vT.z;
			targetBone->Local(F);
		}
	}

}

void UIBoneTransform::ChangeTarget(ModelBone * targetBone, GameModel* targetModel, class ModelClip* targetClip)
{
	if (targetBone == nullptr)
	{
		assert(false);
	}

	this->targetBone = targetBone;
	this->targetModel = targetModel;
	this->targetClip = targetClip;

	strS = "Scale##BT" + to_string(this->targetBone->Index());
	strR = "Rotation##BT" + to_string(this->targetBone->Index());
	strT = "Position##BT" + to_string(this->targetBone->Index());
}

void UIBoneTransform::ChangeContainUIName(string containUIName)
{
	this->containUIName = containUIName;
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
