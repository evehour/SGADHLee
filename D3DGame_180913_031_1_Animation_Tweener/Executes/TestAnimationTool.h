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

private:
	void OpenFbxFile(wstring fName);
	void ExtractFbx(bool isCancel);
};