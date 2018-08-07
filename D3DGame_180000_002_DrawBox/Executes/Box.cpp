#include "stdafx.h"
#include "Box.h"
#include "DrawTexture.h"

Box::Box(ExecuteValues * values)
	: Execute(values)
	, m_YPR(0,0,0), m_position(0,0,0), m_scale(1,1,1)
	, m_allUV(0,0)
{
	for (int i = 0; i < 6; i++)
	{
		m_pDT[i] = new DrawTexture(values, D3DXVECTOR3(0, 0, 0));
		m_fFileName1[i] = bind(&DrawTexture::SetTextureFileName1, m_pDT[i], placeholders::_1);
		m_fFileName2[i] = bind(&DrawTexture::SetTextureFileName2, m_pDT[i], placeholders::_1);
		m_uv[i] = D3DXVECTOR2(0.5f, 0.5f);
	}

	D3DXMatrixIdentity(&root);

	D3DXMATRIX S, R, T;
	// front
	D3DXMatrixTranslation(&squarWorld[0], 0, 0, -0.5f);
	// left
	D3DXMatrixRotationYawPitchRoll(&R, Math::ToRadian(-90), 0, 0);
	D3DXMatrixTranslation(&T, 0.5f, 0, 0);
	squarWorld[1] = R * T;

	// right
	D3DXMatrixRotationYawPitchRoll(&R, Math::ToRadian(90), 0, 0);
	D3DXMatrixTranslation(&T, -0.5f, 0, 0);
	squarWorld[2] = R * T;
	// back
	D3DXMatrixRotationYawPitchRoll(&R, Math::ToRadian(-180), 0, 0);
	D3DXMatrixTranslation(&T, 0, 0, 0.5f);
	squarWorld[3] = R * T;
	// up
	D3DXMatrixRotationYawPitchRoll(&R, 0, Math::ToRadian(90), 0);
	D3DXMatrixTranslation(&T, 0, 0.5f, 0);
	squarWorld[4] = R * T;
	// down
	D3DXMatrixRotationYawPitchRoll(&R, 0, Math::ToRadian(-90), 0);
	D3DXMatrixTranslation(&T, 0, -0.5f, 0);
	squarWorld[5] = R * T;

	for (int i = 0; i < 6; i++)
	{
		m_pDT[i]->SetWorldBuffer(root * squarWorld[i]);
	}
}

Box::~Box()
{
	for (int i = 0; i < 6; i++)
		SAFE_DELETE(m_pDT[i]);
}

void Box::Update()
{
	D3DXMATRIX S, R, T;
	if (Keyboard::Get()->Press(VK_SHIFT))
	{
		if (Keyboard::Get()->Press('A'))
		{
			m_YPR.y += 1.0f * Time::Get()->Delta();
		}
		else if (Keyboard::Get()->Press('D'))
		{
			m_YPR.y -= 1.0f * Time::Get()->Delta();
		}
		else if (Keyboard::Get()->Press('W'))
		{
			m_YPR.x += 1.0f * Time::Get()->Delta();
		}
		else if (Keyboard::Get()->Press('S'))
		{
			m_YPR.x -= 1.0f * Time::Get()->Delta();
		}
	}
	
	if (Keyboard::Get()->Press(VK_LCONTROL))
	{
		if (Keyboard::Get()->Press(VK_LEFT))
		{
			m_scale.x -= 0.1f * Time::Get()->Delta();
		}
		else if (Keyboard::Get()->Press(VK_RIGHT))
		{
			m_scale.x += 0.1f * Time::Get()->Delta();
		}
		if (Keyboard::Get()->Press(VK_UP))
		{
			m_scale.y += 0.1f * Time::Get()->Delta();
		}
		else if (Keyboard::Get()->Press(VK_DOWN))
		{
			m_scale.y -= 0.1f * Time::Get()->Delta();
		}
		if (Keyboard::Get()->Press('N'))
		{
			m_scale.z += 0.1f * Time::Get()->Delta();
		}
		if (Keyboard::Get()->Press('M'))
		{
			m_scale.z -= 0.1f * Time::Get()->Delta();
		}
	}
	else
	{
		if (Keyboard::Get()->Press(VK_LEFT))
		{
			m_position.x -= 1.0f * Time::Get()->Delta();
		}
		else if (Keyboard::Get()->Press(VK_RIGHT))
		{
			m_position.x += 1.0f * Time::Get()->Delta();
		}
		else if (Keyboard::Get()->Press(VK_UP))
		{
			m_position.y += 1.0f * Time::Get()->Delta();
		}
		else if (Keyboard::Get()->Press(VK_DOWN))
		{
			m_position.y -= 1.0f * Time::Get()->Delta();
		}
	}
	D3DXMatrixScaling(&S, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixRotationYawPitchRoll(&R, m_YPR.y, m_YPR.x, m_YPR.z);
	D3DXMatrixTranslation(&T, m_position.x, m_position.y, m_position.z);

	root = S * R * T;

	for (int i = 0; i < 6; i++)
	{
		m_pDT[i]->SetWorldBuffer(squarWorld[i] * root);
	}
}

void Box::PreRender()
{
}

void Box::Render()
{
	for (int i = 0; i < 6; i++)
	{
		m_pDT[i]->Render();
	}
}

void Box::PostRender()
{
	ImGui::Begin("Color");
	{
		ImGui::SliderFloat2("All Setting UV", (float *)&m_allUV, 0, 1);
		//ImGui::SliderFloat3("Color", (float *)&colorBuffer->Data.Color, 0, 1);
		for (int i = 0; i < 6; i++)
		{
			char str[32];
			sprintf(str, "Texture1 - %02d", i);
			if (ImGui::Button(str))
			{
				Path::OpenFileDialog(L"Select texture1", L"PNG files\0*.png\0", Textures, m_fFileName1[i]);
			}
			sprintf(str, "Texture2 - %02d", i);
			if (ImGui::Button(str))
			{
				Path::OpenFileDialog(L"Select texture1", L"PNG files\0*.png\0", Textures, m_fFileName2[i]);
			}

			sprintf(str, "UV%02d", i);
			ImGui::SliderFloat2(str, (float *)&m_uv[i], 0, 1);
			m_pDT[i]->SetUV(m_uv[i]);
		}

		if (m_allUV.y > 0.0f)
		{
			for (int i = 0; i < 6; i++)
			{
				m_pDT[i]->SetUV(m_allUV);
			}
		}
	}
	ImGui::End();
}

void Box::ResizeScreen()
{
}
