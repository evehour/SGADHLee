#include "stdafx.h"
#include "ExeHW.h"
#include "../Objects/MeshObject.h"

ExeHW::ExeHW(ExecuteValues * values)
	: Execute(values)
	, pickObj(NULL), pickIdx(-1)
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

		pickIdx = -1;
		for (int i = 1; i < objects.size(); i++)
		{
#if false
			if (IsPick(objects[i]))
			{
				pickObj = objects[i];
				pickIdx = i;
				break;
			}
#else
			if (objects[i]->IsPick(start, direction))
			{
				pickObj = objects[i];
				pickIdx = i;
				break;
			}
#endif
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
		ImGui::LabelText("Picked", "%d", pickIdx);
		ImGui::Separator();
		if (pickIdx >= 0)
		{
			D3DXVECTOR3 _pos(0, 0, 0);
			_pos = objects[pickIdx]->Position();
			ImGui::SliderFloat3("Position", (float*)&_pos, -10.0f, 10.0f);
			objects[pickIdx]->Position(_pos);
		}
	}
	ImGui::End();
}

void ExeHW::ResizeScreen()
{
}

bool ExeHW::IsPick(MeshObject * object)
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
				return true;
		}
	}
	return false;
}

