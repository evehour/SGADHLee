#include "stdafx.h"
#include "ExeHW.h"
#include "../Objects/MeshObject.h"

ExeHW::ExeHW(ExecuteValues * values)
	: Execute(values)
	, pickObj(NULL), pickIdx(-1), toolState(0), createObjectType(0)
{
	MeshObject *i;


	i = new MeshObject
	(
		Materials + L"Meshes/", L"Plane.material",
		Models + L"Meshes/", L"Plane.mesh",
		D3DXCOLOR(1, 1, 1, 1)
	);
	i->Position(D3DXVECTOR3(0, 0, 0));
	objects.push_back(i);

	i = new MeshObject
	(
		Materials + L"Meshes/", L"Cylinder.material",
		Models + L"Meshes/", L"Cylinder.mesh",
		D3DXCOLOR(1, 0, 0, 1)
	);
	i->Position(D3DXVECTOR3(3, 0, 0));
	objects.push_back(i);
	
	i = new MeshObject
	(
		Materials + L"Meshes/", L"Cube.material",
		Models + L"Meshes/", L"Cube.mesh",
		D3DXCOLOR(0, 1, 0, 1)
	);
	i->Position(D3DXVECTOR3(0, 3, 0));
	objects.push_back(i);

	i = new MeshObject
	(
		Materials + L"Meshes/", L"Sphere.material",
		Models + L"Meshes/", L"Sphere.mesh",
		D3DXCOLOR(0, 0, 1, 1)
	);
	i->Position(D3DXVECTOR3(-3, 0, 0));
	objects.push_back(i);

	pickPosition = D3DXVECTOR3(0, 0, 0);
	objectDiffuseColor = D3DXCOLOR(1, 1, 1, 1);
}

ExeHW::~ExeHW()
{
	for (MeshObject* object : objects)
		SAFE_DELETE(object);
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
			pickIdx = -1;

			for (int i = 1; i < objects.size(); i++)
			{
#if false
				if (IsPick(objects[i], pickPosition))
				{
					pickObj = objects[i];
					pickIdx = i;
					break;
				}
#else
				if (objects[i]->IsPick(start, direction, pickPosition))
				{
					pickObj = objects[i];
					pickIdx = i;
					break;
				}
#endif
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
		ImGui::SliderInt("", &toolState, 0, (int)(TOOL_STATE_MAX - 1));
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
			if (pickIdx >= 0)
			{
				D3DXVECTOR3 _pos(0, 0, 0);
				//ImGui::Text("Picked Object Index:");
				ImGui::LabelText("Object Index", "%d", pickIdx);
				ImGui::Separator();
				_pos = objects[pickIdx]->Position();
				ImGui::LabelText("Pick Pos", "%.2f, %.2f, %.2f", pickPosition.x, pickPosition.y, pickPosition.z);
				ImGui::SliderFloat3("Position", (float*)&_pos, -10.0f, 10.0f);
				objects[pickIdx]->Position(_pos);
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

