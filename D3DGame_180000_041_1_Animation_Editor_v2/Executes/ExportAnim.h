#pragma once
#include "Execute.h"

class ExportAnim : public Execute
{
public:
	ExportAnim(ExecuteValues* values);
	~ExportAnim();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	
};