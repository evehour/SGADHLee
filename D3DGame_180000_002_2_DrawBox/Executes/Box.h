#pragma once
#include "Execute.h"

class Box : public Execute
{
private:
	class DrawTexture* m_pDT[6];
	function<void(wstring)> m_fFileName1[6], m_fFileName2[6];

	D3DXMATRIX squarWorld[6];
	D3DXMATRIX root;

	D3DXVECTOR3 m_YPR, m_position, m_scale;


	D3DXVECTOR2 m_uv[6];
	D3DXVECTOR2 m_allUV;

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