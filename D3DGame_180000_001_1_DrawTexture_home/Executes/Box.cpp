#include "stdafx.h"
#include "Box.h"
#include "DrawTexture.h"

Box::Box(ExecuteValues * values)
	: Execute(values)
{
	m_pDT = new DrawTexture(values);
}

Box::~Box()
{
	SAFE_DELETE(m_pDT);
}

void Box::Update()
{
}

void Box::PreRender()
{
}

void Box::Render()
{
	
}

void Box::PostRender()
{
	ImGui::Begin("Color");
	{
		//ImGui::SliderFloat3("Color", (float *)&colorBuffer->Data.Color, 0, 1);
	}
	ImGui::End();
}

void Box::ResizeScreen()
{
}
