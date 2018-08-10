#include "stdafx.h"
#include "./Executes/Sphere.h"
#include "./Executes/Terrain.h"
#include "ExeHW1.h"

ExeHW1::ExeHW1(ExecuteValues * values)
	: Execute(values)
{
	int width, height, center;

	cameraMode = 0;

	width = 256;
	height = 256;
	center = ((width * height) / 2) - (width / 2);

	m_terrain = new Terrain(values);

	m_terrain->GetCellPosition(center, &spherePos);
	m_sphere = new Sphere(values);
	m_sphere->SetPosition(spherePos);

	D3DXMatrixIdentity(&matCam);
	D3DXMatrixIdentity(&matCenter);

	D3DXMatrixTranslation(&matCenter, spherePos.x, spherePos.y, spherePos.z);

	rotation = D3DXVECTOR3(0, 0, 0);

	values->MainCamera->Position(&initCamPos);
	initCamPos.x = 0;
	initCamPos.y = 0;
	D3DXMatrixTranslation(&matCam, initCamPos.x, initCamPos.y, initCamPos.z);
}

ExeHW1::~ExeHW1()
{
	SAFE_DELETE(m_sphere);
	SAFE_DELETE(m_terrain);
}

void ExeHW1::Update()
{

	if (cameraMode == 1)
	{
		float rotateSpeed = 1.0f;
		float moveSpeed = 10.0f;
		
		if (Keyboard::Get()->Press('A'))
		{
			rotation.y += rotateSpeed * Time::Get()->Delta();
		}
		else if (Keyboard::Get()->Press('D'))
		{
			rotation.y -= rotateSpeed * Time::Get()->Delta();
		}

		if (Keyboard::Get()->Press('Q'))
		{
			rotation.x += rotateSpeed * Time::Get()->Delta();
		}
		else if (Keyboard::Get()->Press('E'))
		{
			rotation.x -= rotateSpeed * Time::Get()->Delta();
		}

		if (Keyboard::Get()->Press('W'))
		{
			initCamPos.z += moveSpeed * Time::Get()->Delta();
			D3DXMatrixTranslation(&matCam, initCamPos.x, initCamPos.y, initCamPos.z);
		}
		else if (Keyboard::Get()->Press('S'))
		{
			initCamPos.z -= moveSpeed * Time::Get()->Delta();
			D3DXMatrixTranslation(&matCam, initCamPos.x, initCamPos.y, initCamPos.z);
		}

		D3DXMATRIX matFinal, matRot;
		D3DXMatrixRotationYawPitchRoll(&matRot, rotation.y, rotation.x, rotation.z);
		matRot = matRot * matCenter;
		matFinal = matCam * matRot;
		values->MainCamera->Position(matFinal._41, matFinal._42, matFinal._43);
		values->MainCamera->Rotation(rotation.x, rotation.y);
	}

	m_sphere->Update();
	m_terrain->Update();
}

void ExeHW1::PreRender()
{
	m_sphere->PreRender();
	m_terrain->PreRender();

}

void ExeHW1::Render()
{
	m_sphere->Render();
	m_terrain->Render();
}

void ExeHW1::PostRender()
{
	ImGui::SliderInt("Orbit camera to Sphere", &cameraMode, 0, 1);

	m_sphere->PostRender();
	m_terrain->PostRender();
}

void ExeHW1::ResizeScreen()
{
}

