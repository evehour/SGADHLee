#include "stdafx.h"
#include "SpotLight.h"
#include "../Objects/DebugDraw.h"

SpotLight::SpotLight(ExecuteValues * values)
	:values(values), pickedIdx(-1)
{
	buffer = new Buffer();

	LightPackage temp;
	vLights.assign(32, temp);

	mScale = D3DXVECTOR3(1, 1, 1);
}

SpotLight::~SpotLight()
{
	for (UINT i = 0; i < vLights.size(); i++)
		SAFE_DELETE(vLights[i].debugDraw);
	SAFE_DELETE(buffer);
}

void SpotLight::Update()
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

void SpotLight::Render()
{
	buffer->SetPSBuffer(3);

	for (UINT i = 0; i < vLights.size(); i++)
	{
		if (vLights[i].debugDraw == NULL) continue;
		vLights[i].debugDraw->Render();
	}
}

void SpotLight::PostRender()
{
	ImGui::Begin("Spot Light");
	{
		if (pickedIdx >= 0)
		{
			ImGui::SliderFloat3("Position", (float *)&vLights[pickedIdx].desc.Position, 0, 256);
			ImGui::SliderFloat3("Color", (float *)&vLights[pickedIdx].desc.Color, 0, 1);
			ImGui::SliderFloat("Outer Angle", &vLights[pickedIdx].desc.OuterAngle, 0, 1);
		}
	}
	ImGui::End();
}

void SpotLight::Add(Desc & desc)
{
	int count = buffer->Data.Count;
	buffer->Data.Lights[count] = desc;

	LightPackage temp;
	temp.idx = count;
	temp.desc = desc;
	temp.debugDraw = new DebugDraw();
	temp.debugDraw->Position(desc.Position);
	temp.debugDraw->Scale(mScale);
	temp.debugDraw->Update();
	vLights[count] = temp;

	buffer->Data.Count++;
}
