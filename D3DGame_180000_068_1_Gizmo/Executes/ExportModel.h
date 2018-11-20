#pragma once
#include "Execute.h"

class ExportModel : public Execute
{
public:
	ExportModel(ExecuteValues* values);
	~ExportModel();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	
};