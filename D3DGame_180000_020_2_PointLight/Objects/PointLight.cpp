#include "stdafx.h"
#include "PointLight.h"
#include "../Objects/DebugDraw.h"

PointLight::PointLight(ExecuteValues * values)
	:values(values), pickedIdx(-1)
{
	buffer = new Buffer();
	pointLightPackage temp;
	vPointLights.assign(32, temp);
}

PointLight::~PointLight()
{
	for (UINT i = 0; i < vPointLights.size(); i++)
		SAFE_DELETE(vPointLights[i].debugDraw);
	SAFE_DELETE(buffer);
}

void PointLight::Update()
{
	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);

	D3DXVECTOR3 direction = values->MainCamera->Direction(
		values->Viewport, values->Perspective
	);
	D3DXVECTOR3 pickedPos;

	bool isClick = false;
	bool isPick = false;

	if (Keyboard::Get()->Press(VK_LSHIFT) && Mouse::Get()->Down(0))
		isClick = true;

	for (UINT i = 0; i < vPointLights.size(); i++)
	{
		if (vPointLights[i].debugDraw == NULL) continue;
		if (isClick)
		{
			if (!isPick)
			{
				if (vPointLights[i].debugDraw->IsPick(start, direction, pickedPos))
				{
					pickedIdx = i;
					isPick = true;
				}
			}
		}

		UINT idx = vPointLights[i].idx;
		vPointLights[i].debugDraw->Position(vPointLights[i].desc.Position);

		float s = vPointLights[i].desc.Range;
		vPointLights[i].debugDraw->Scale(s,s,s);

		D3DXVECTOR3 dc = vPointLights[i].desc.Color;
		D3DXCOLOR ddc = D3DXCOLOR(dc.x, dc.y, dc.z, 1);
		vPointLights[i].debugDraw->SetColor(ddc);

		vPointLights[i].debugDraw->Update();

		buffer->Data.Lights[idx] = vPointLights[i].desc;
	}

	if (isClick)
	{
		pickedIdx = isPick ? pickedIdx : -1;
	}
}

void PointLight::Render()
{
	buffer->SetPSBuffer(2);
	for (UINT i = 0; i < vPointLights.size(); i++)
	{
		if (vPointLights[i].debugDraw == NULL) continue;
		vPointLights[i].debugDraw->Render();
	}
}

void PointLight::PostRender()
{
	ImGui::Begin("Point Light");
	{
		if (pickedIdx >= 0)
		{
			ImGui::SliderFloat3("Position", (float *)&vPointLights[pickedIdx].desc.Position, 0, 256);
			ImGui::SliderFloat3("Color", (float *)&vPointLights[pickedIdx].desc.Color, 0, 1);
			ImGui::SliderFloat("Range", &vPointLights[pickedIdx].desc.Range, 0, 10);
			ImGui::SliderFloat("Intensity", &vPointLights[pickedIdx].desc.Intensity, 0.001f, 10);
		}
	}
	ImGui::End();
}

void PointLight::Add(Desc & desc)
{
	int count = buffer->Data.Count;
	buffer->Data.Lights[count] = desc;
	
	pointLightPackage temp;
	temp.idx = count;
	temp.desc = desc;
	temp.debugDraw = new DebugDraw();
	temp.debugDraw->Position(desc.Position);
	temp.debugDraw->Scale(D3DXVECTOR3(desc.Range, desc.Range, desc.Range));
	temp.debugDraw->Update();
	vPointLights[count] = temp;

	buffer->Data.Count++;
}
