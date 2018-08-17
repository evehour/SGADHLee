#pragma once
#include "Execute.h"

enum TOOL_STATE
{
	TOOL_STATE_SELECT,
	TOOL_STATE_MODIFY,
	TOOL_STATE_CREATE_OBJECT,
	TOOL_STATE_MAX
};

class ExeHW : public Execute
{
public:
	ExeHW(ExecuteValues* values);
	~ExeHW();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	bool IsPick(class MeshObject* object, OUT D3DXVECTOR3 & position);

private:
	vector<class MeshObject *> objects;
	class MeshObject* pickObj;

	int pickBoneIdx;
	D3DXVECTOR3 _s, _r, _t;
	D3DXQUATERNION _qr;
	float _angle;

	D3DXVECTOR3 pickPosition;

	int toolState;
	int createObjectType;
	D3DXCOLOR objectDiffuseColor;

};