#pragma once
#include "Execute.h"

class DebugDraw : public Execute
{
public:
	DebugDraw(ExecuteValues* values, UINT objNum = 0);
	~DebugDraw();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	void ChangeObj(UINT objNum);

	D3DXVECTOR3 Position() { return position; }
	void Position(D3DXVECTOR3& pos) { position = pos; }

	D3DXVECTOR3 Scale() { return scale; }
	void Scale(D3DXVECTOR3& scl) { scale = scl; }

	D3DXVECTOR3 Rotation() { return rotation; }
	void Rotation(D3DXVECTOR3& rot) { rotation = rot; }

private:
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	class LineMake* line;

	vector<VertexTextureNormal>* vTargetVertices;
	vector<UINT>* vTargetIndices;

private:
	UINT objNum;
	vector<VertexTextureNormal> vSphereVertices;
	vector<UINT> vSphereIndices;
	vector<VertexTextureNormal> vCubeVertices;
	vector<UINT> vCubeIndices;

	void SettingObj();
};