#include "stdafx.h"
#include "TestAnimationTool.h"
#include "../Objects/GameAnimModel.h"

#include "../Fbx/Exporter.h"

TestAnimationTool::TestAnimationTool(ExecuteValues * values)
	: Execute(values)
	, targetModel(nullptr)
	, animExportIdxArray(nullptr)
	, bOpenFbxAttr(false), bFileLoadComplete(false)
	, attrDataFlag(0), extDataFlag(0)
{

}

TestAnimationTool::~TestAnimationTool()
{

}

void TestAnimationTool::Update()
{
	if(targetModel != nullptr) targetModel->Update();
}

void TestAnimationTool::PreRender()
{

}

void TestAnimationTool::Render()
{
	if (targetModel != nullptr) targetModel->Render();
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
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
				Path::OpenFileDialog
				(
					L""
					, L"FBX Files(*.fbx)\0 * .fbx\0"
					, Assets
					, bind(&TestAnimationTool::OpenFbxFile, this, placeholders::_1)
				);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// Pre-Extract process
	if(bOpenFbxAttr)
	{
		ImGui::Begin("Export Option");
		{
			if (attrDataFlag & 1) ImGui::CheckboxFlags("Material", &extDataFlag, 1);
			if (attrDataFlag & 2) ImGui::CheckboxFlags("Mesh & Bone", &extDataFlag, 2);
			if (attrDataFlag & 4)
			{
				ImGui::CheckboxFlags("Animation", &extDataFlag, 4);
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
		ImGui::End();
	}

	// Post-Load
	if(bFileLoadComplete)
	{
		ImGui::Begin("Hireacy");
		{
			ImGui::SetWindowSize(ImVec2(200, 668));
			ImGui::SetWindowPos(ImVec2(16, 34));

			if (ImGui::CollapsingHeader("Material"))
			{
				if (ImGui::TreeNode("Basic"))
				{
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();

		ImGui::Begin("Inspector");
		{
			ImGui::SetWindowPos(ImVec2(956, 34));
			ImGui::SetWindowSize(ImVec2(300, 668));

		}
		ImGui::End();
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
	if (attrDataFlag & 4)
	{
		animExportIdxArray = new bool[animationNames.size()];
		memset(animExportIdxArray, false, animationNames.size());
	}
	SAFE_DELETE(exporter);

	return;
}

void TestAnimationTool::ExtractFbx(bool isCancel)
{
	Fbx::Exporter* exporter = NULL;
	bOpenFbxAttr = false;

	if (isCancel) return;
	if (extDataFlag == 4)
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

	//wstring fileName = Path::GetFileNameWithoutExtension(selectedFbxFileName);
	//wstring folderName = Path::GetDirectoryName(selectedFbxFileName);

	//wstring a, b;
	//a = folderName + fileName + L"/";

	//if (extDataFlag & 1)
	//{
	//	b = fileName + L".material";
	//	exporter = new Fbx::Exporter(selectedFbxFileName);
	//	exporter->ExportMaterial(a, b);
	//	SAFE_DELETE(exporter);
	//}

	//if (extDataFlag & 2)
	//{
	//	b = fileName + L".mesh";
	//	exporter = new Fbx::Exporter(selectedFbxFileName);
	//	exporter->ExportMaterial(a, b);
	//	SAFE_DELETE(exporter);
	//}

	//if (extDataFlag & 4)
	//{
	//	exporter = new Fbx::Exporter(selectedFbxFileName, false);
	//	for (UINT i = 0; i < animationNames.size(); i++)
	//	{
	//		if (animExportIdxArray[i])
	//		{
	//			b = fileName + L"_" + String::ToWString(animationNames[i]) + L".anim";
	//			exporter->ExportMaterial(a, b);
	//		}
	//	}
	//	SAFE_DELETE_ARRAY(animExportIdxArray);
	//	SAFE_DELETE(exporter);
	//}

	//if (targetModel != nullptr)
	//{
	//	SAFE_DELETE(targetModel);
	//}
	//targetModel = new GameAnimModel(a, fileName + L".material", a, fileName + L".mesh");

	bFileLoadComplete = true;

	return;
}

