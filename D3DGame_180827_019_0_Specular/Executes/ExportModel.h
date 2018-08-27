#pragma once
#include "Execute.h"

class ExportModel : public Execute
{
public:
	ExportModel();
	~ExportModel();

	void Update();
	void PreRender();
	void Render();
	void Resize();

private:
	
};