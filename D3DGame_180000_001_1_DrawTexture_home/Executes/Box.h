#pragma once
#include "Execute.h"

class Box : public Execute
{
private:
	class DrawTexture* m_pDT;
	function<void(wstring)> m_fFileName[9];

public:
	Box(ExecuteValues* values);
	~Box();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
};