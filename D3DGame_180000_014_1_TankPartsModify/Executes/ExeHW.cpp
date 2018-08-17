#include "stdafx.h"
#include "ExeHW.h"
#include "../Objects/MeshObject.h"

ExeHW::ExeHW(ExecuteValues * values)
	: Execute(values)
	, pickObj(NULL), pickBoneIdx(-1), toolState(0), createObjectType(0)
{
	_s = D3DXVECTOR3(1, 1, 1);
	_r = D3DXVECTOR3(0, 0, 0);
	_t = D3DXVECTOR3(0, 0, 0);
	_angle = 0.0f;

	MeshObject *i = new MeshObject
	(
		Models + L"Tank/", L"Tank.material",
		Models + L"Tank/", L"Tank.mesh",
		D3DXCOLOR(1, 1, 1, 1)
	);
	objects.push_back(i);
}

ExeHW::~ExeHW()
{
	for (MeshObject* object : objects)
		SAFE_DELETE(object);
}

static void toEulerAngle(const D3DXQUATERNION& q, double& pitch, double& yaw, double& roll)
{
	// pitch (x-axis rotation)
	double sinr = +2.0 * (q.w * q.x + q.y * q.z);
	double cosr = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
	pitch = atan2(sinr, cosr);

	// yaw (y-axis rotation)
	double sinp = +2.0 * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1)
		yaw = copysign(D3DX_PI / 2, sinp); // use 90 degrees if out of range
	else
		yaw = asin(sinp);

	// roll (z-axis rotation)
	double siny = +2.0 * (q.w * q.z + q.x * q.y);
	double cosy = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	roll = atan2(siny, cosy);
}

void ExeHW::Update()
{

	if (Keyboard::Get()->Press(VK_LSHIFT) && Mouse::Get()->Down(0))
	{
		D3DXVECTOR3 start;
		values->MainCamera->Position(&start);

		D3DXVECTOR3 direction = values->MainCamera->Direction(
			values->Viewport, values->Perspective
		);

		switch (toolState)
		{
		case (int)TOOL_STATE_CREATE_OBJECT:
			if (objects[0]->IsPick(start, direction, pickPosition))
			{
				wstring objFileName;
				if (createObjectType == 0) objFileName = L"Sphere";
				if (createObjectType == 1) objFileName = L"Cube";
				if (createObjectType == 2) objFileName = L"Cylinder";
				if (createObjectType == 3) objFileName = L"Capsule";

				MeshObject *obj;
				obj = new MeshObject
				(
					Materials + L"Meshes/", objFileName + L".material",
					Models + L"Meshes/", objFileName + L".mesh",
					objectDiffuseColor
				);
				obj->Position(pickPosition);
				objects.push_back(obj);
			}
			break;
		case (int)TOOL_STATE_SELECT:
			pickBoneIdx = -1;
			if (objects[0]->IsPickPart(start, direction, pickBoneIdx, pickPosition))
			{
				D3DXMATRIX matLocal = objects[0]->GetBoneLocalMatrixByIdx(pickBoneIdx);
				D3DXVECTOR3 _vScl, _vRot, _vPos;
				D3DXMatrixDecompose(&_s, &_qr, &_t, &matLocal);
				double y, p, r;
				toEulerAngle(_qr, p, y, r);
				_r.x = (float)p;
				_r.y = (float)y;
				_r.z = (float)r;
			}
			break;
		}
	}

	for (MeshObject* object : objects)
		object->Update();
}

void ExeHW::PreRender()
{

}

void ExeHW::Render()
{
	for (MeshObject* object : objects)
		object->Render();
}

void ExeHW::PostRender()
{
	ImGui::Begin("Color");
	{
		char _str[256]{ 0 };

		switch (toolState)
		{
		case 0:
			sprintf_s(_str, "Create Object");
			break;
		case 1:
			sprintf_s(_str, "Select Object");
			break;
		}
		ImGui::Text("Current Mode: %s", _str);
		//ImGui::SliderInt("", &toolState, 0, (int)(TOOL_STATE_MAX - 1));
		ImGui::Separator();

		switch (toolState)
		{
		case (int)TOOL_STATE_CREATE_OBJECT:
			ImGui::ColorEdit4("Albedo", (float*)&objectDiffuseColor);
			if (ImGui::Button("Make Sphere")) createObjectType = 0;
			else if (ImGui::Button("Make Cube")) createObjectType = 1;
			else if (ImGui::Button("Make Cylinder")) createObjectType = 2;
			else if (ImGui::Button("Make Capsule")) createObjectType = 3;
			break;
		case (int)TOOL_STATE_SELECT:
			if (pickBoneIdx >= 0)
			{
				string n = objects[0]->GetBoneNameByIdx(pickBoneIdx);

				ImGui::LabelText("Object Index", "%d", pickBoneIdx);
				ImGui::LabelText("Object Name", "%s", n.c_str());
				ImGui::Separator();
				ImGui::SliderFloat3("Position", (float*)&_t, -100.0f, 100.0f);
				ImGui::SliderFloat3("Rotation", (float*)&_r, -10.0f, 10.0f);
				ImGui::SliderFloat3("Scale",	(float*)&_s, -10.0f, 10.0f);

				D3DXMATRIX S, R, T, F;
				D3DXMATRIX X, Y, Z;
				
				D3DXMatrixScaling(&S, _s.x, _s.y, _s.z);
				D3DXMatrixRotationYawPitchRoll(&R, _r.y, _r.x, _r.z);
				D3DXMatrixTranslation(&T, _t.x, _t.y, _t.z);
				F = S * R * T;
				objects[0]->SetBoneMatrixByIdx(pickBoneIdx, F);
			}
			break;
		}
	}
	ImGui::End();
}

void ExeHW::ResizeScreen()
{
}

bool ExeHW::IsPick(MeshObject * object, OUT D3DXVECTOR3 & position)
{
	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);

	D3DXVECTOR3 direction = values->MainCamera->Direction(
		values->Viewport, values->Perspective
	);

	std::vector<D3DXVECTOR3> boxTri;
	object->GetBoundSpaceTries(boxTri);

	D3DXMATRIX w = object->World();
	D3DXMATRIX invW;
	D3DXMatrixInverse(&invW, NULL, &w);

	D3DXVec3TransformCoord(&start, &start, &invW);
	D3DXVec3TransformNormal(&direction, &direction, &invW);
	D3DXVec3Normalize(&direction, &direction);

	D3DXVECTOR3 pickPos;
	for (int i = 0; i < 6; i++)
	{
		D3DXVECTOR3 p[6];
		float u, v, distance;
		for (int j = 0; j < 6; j++)
		{
			p[j] = boxTri[i * 6 + j];
		}

		for (int j = 0; j < 2; j++)
		{
			if (D3DXIntersectTri(
				&p[j * 3 + 0], &p[j * 3 + 1], &p[j * 3 + 2],
				&start, &direction,
				&u, &v, &distance))
			{
				pickPos = p[j * 3 + 0] + (u * (p[j * 3 + 1] - p[j * 3 + 0])) + (v * (p[j * 3 + 2] - p[j * 3 + 0]));
				D3DXVec3TransformCoord(&pickPos, &pickPos, &w);
				position = pickPos;
				return true;
			}
		}
	}
	return false;
}

