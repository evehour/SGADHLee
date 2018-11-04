#pragma once
#include "GameModel.h"

class DebugDraw : public GameModel
{
public:
	enum DRAW_OBJECT_TYPE
	{
		DRAW_OBJECT_TYPE_SPHERE = 0,
		DRAW_OBJECT_TYPE_CUBE,
		DRAW_OBJECT_TYPE_CAPSULE,
		DRAW_OBJECT_TYPE_MAX
	};

public:
	DebugDraw(DRAW_OBJECT_TYPE objNum = DRAW_OBJECT_TYPE_SPHERE);
	~DebugDraw();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();


	void ChangeObj(DRAW_OBJECT_TYPE objNum);

	D3DXCOLOR GetColor();
	void SetColor(D3DXCOLOR& color);

	/*
	D3DXVECTOR3 Position() { return position; }
	void Position(D3DXVECTOR3& pos) { position = pos; }

	D3DXVECTOR3 Scale() { return scale; }
	void Scale(D3DXVECTOR3& scl) { scale = scl; }

	D3DXVECTOR3 Rotation() { return rotation; }
	void Rotation(D3DXVECTOR3& rot) { rotation = rot; }
	*/

private:
	/*D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;*/

	class LineMake* line;
	class LineMake* compass[3];

	vector<VertexTextureNormal>* vTargetVertices;
	vector<UINT>* vTargetIndices;

private:
	DRAW_OBJECT_TYPE objNum;

	vector<VertexTextureNormal> vSphereVertices;
	vector<UINT> vSphereIndices;

	vector<VertexTextureNormal> vCubeVertices;
	vector<UINT> vCubeIndices;

	vector<VertexTextureNormal> vCapsuleVertices;
	vector<UINT> vCapsuleIndices;

	void SettingObj();
	void SettingObjCube();
	void SettingObjSphere();
	void SettingObjCapsule();
	void SettingObjCapsule2();
};