#include "stdafx.h"
#include "AreaLight.h"
#include "../Objects/DebugDraw.h"

AreaLight::AreaLight(ExecuteValues * values)
	:values(values), pickedIdx(-1)
{
	buffer = new Buffer();

	LightPackage temp;
	vLights.assign(32, temp);

	mScale = D3DXVECTOR3(1, 1, 1);
}

AreaLight::~AreaLight()
{
	for (UINT i = 0; i < vLights.size(); i++)
		SAFE_DELETE(vLights[i].debugDraw);

	SAFE_DELETE(buffer);
}

void AreaLight::Update()
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

	for (UINT i = 0; i < vLights.size(); i++)
	{
		if (vLights[i].debugDraw == NULL) continue;
		if (isClick)
		{
			if (!isPick)
			{
				if (vLights[i].debugDraw->IsPick(start, direction, pickedPos))
				{
					pickedIdx = i;
					isPick = true;
				}
			}
		}

		UINT idx = vLights[i].idx;
		vLights[i].debugDraw->Position(vLights[i].desc.Position);

		vLights[i].debugDraw->Scale(mScale);

		D3DXVECTOR3 dc = vLights[i].desc.Color;
		D3DXCOLOR ddc = D3DXCOLOR(dc.x, dc.y, dc.z, 1);
		vLights[i].debugDraw->SetColor(ddc);

		vLights[i].debugDraw->Update();

		buffer->Data.Lights[idx] = vLights[i].desc;
	}

	if (isClick)
	{
		pickedIdx = isPick ? pickedIdx : -1;
	}
}

void AreaLight::Render()
{
	buffer->SetPSBuffer(4);
	for (UINT i = 0; i < vLights.size(); i++)
	{
		if (vLights[i].debugDraw == NULL) continue;
		vLights[i].debugDraw->Render();
	}
}

void AreaLight::PostRender()
{
	ImGui::Begin("Area Light");
	{
		if (pickedIdx >= 0)
		{
			ImGui::SliderFloat3("Position", (float *)&vLights[pickedIdx].desc.Position, 0, 256);
			ImGui::SliderFloat3("Color", (float *)&vLights[pickedIdx].desc.Color, 0, 1);
			ImGui::SliderFloat("AreaLightWidth", &vLights[pickedIdx].desc.AreaLightWidth, 1, 100);
			ImGui::SliderFloat("AreaLightHeight", &vLights[pickedIdx].desc.AreaLightHeight, 1, 100);
			ImGui::SliderFloat("Intensity", &vLights[pickedIdx].desc.Intensity, 0.001f, 100);
		}
	}
	ImGui::End();
}

void AreaLight::Add(Desc & desc)
{
	int count = buffer->Data.Count;
	buffer->Data.Lights[count] = desc;

	LightPackage temp;
	temp.idx = count;
	temp.desc = desc;
	temp.debugDraw = new DebugDraw();
	temp.debugDraw->Position(desc.Position);
	temp.debugDraw->Scale(D3DXVECTOR3(1, 1, 1));
	temp.debugDraw->Update();
	vLights[count] = temp;

	buffer->Data.Count++;
}
