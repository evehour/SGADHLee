#pragma once
#include "Execute.h"

class TestAnimationTool : public Execute
{
public:
	TestAnimationTool(ExecuteValues* values);
	~TestAnimationTool();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen(){}

private:
	class GameAnimModel* targetModel;

private:
	wstring selectedFbxFileName;

	// Environment
	UINT attrDataFlag;
	UINT extDataFlag;
	bool* animExportIdxArray;
	vector<string> animationNames;

	// FBX file Loading Process
	bool bOpenFbxAttr;

	// Post-Load Process
	bool bFileLoadComplete;

	// Relate-Model
	int selectedTargetBone;

	D3DXVECTOR3 m_S, m_R, m_T;
	D3DXQUATERNION m_qR;

	// Relate-Animation
	int selectedAnimationIdx, selectedAnimationClipsIdx;
	float animationTime, animationBeforeTime;
	float animationPlaySpeed;
	bool isRepeat;
	bool isPlayAnimation;
	bool isHiaracyAnimDown;
	vector<class DebugDraw *> bonePin;
	bool isSaveAnimDataWindowOpen;
	int saveAnimListIdx;

private:
	vector<class GameAnimModel *> models;
	vector<class ModelClip *> clips;
	DepthStencilState* depthStencilState[2];

private:
	void OpenFbxFile(wstring fName);
	void OpenAnimFile(wstring fName);

	void ExtractFbx(bool isCancel);

	void ExportData(OUT D3DXVECTOR3& scale, OUT D3DXQUATERNION& qrot, OUT D3DXVECTOR3& rot, OUT D3DXVECTOR3& pos, OUT D3DXMATRIX localMatrix, const float& time);
	void CalcData(const D3DXVECTOR3& scale, const D3DXVECTOR3& rot, const D3DXVECTOR3& pos, OUT D3DXMATRIX& localMatrix);

	void BoneTreeCreator(class GameAnimModel* model, class ModelBone* bone);

	char _changeAnimationName[1024];

private:
	//ImGui Windows
	void MainMenu();
	void ExportOption();
	void ImguiHierarchy();
	void ImguiInspector();
	void ImguiTimeSpector();
};