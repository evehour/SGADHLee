#include "stdafx.h"
#include "TestFrustum.h"
#include "../Viewer/Fixity.h"
#include "../Viewer/Frustum.h"
#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"

#include "../Physics/LineMake.h"

TestFrustum::TestFrustum(ExecuteValues * values)
	: Execute(values), drawCount(0)
{
	shader = new Shader(Shaders + L"028_Diffuse.hlsl");
	plane = new MeshPlane();
	plane->Scale(10.0f, 10.0f, 10.0f);
	plane->SetShader(shader);
	plane->SetDiffuse(1.0f, 0.0f, 0.0f, 1.0f);

	sphere = new MeshSphere();
	sphere->SetShader(shader);
	sphere->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	
	camera = new Fixity();
	camera->Position(0.0f, 0.0f, -50.0f);


	projection = new Perspective(1024.0f, 768.0f, Math::PI * 0.25f, 1.0f, 1000.0f);

	//frustum = new Frustum(values, 100, camera, projection);
	frustum = new Frustum(values, 100);

	for (float z = -50; z < 50; z += 10)
	{
		for (float y = -50; y < 50; y += 10)
		{
			for (float x = -50; x < 50; x += 10)
			{
				positions.push_back(D3DXVECTOR3(x, y, z));
			}
		}
	}


	//Create DepthStencilState
	{
		depthMode[0] = new DepthStencilState();
		depthMode[1] = new DepthStencilState();
		depthMode[1]->DepthEnable(false); // ±íÀÌ¹öÆÛ ²û.
	}
	drawArea = new LineMake();
	isMultiPick = false;
}

TestFrustum::~TestFrustum()
{
	SAFE_DELETE(drawArea);
	SAFE_DELETE(depthMode[0]);
	SAFE_DELETE(depthMode[1]);

	SAFE_DELETE(shader);
}

void TestFrustum::Update()
{
	D3DXVECTOR3 box[4];
	frustum->Update();

	if (Mouse::Get()->Down(0))
	{
		isMultiPick = false;
		pickMousePos[0] = Mouse::Get()->GetPosition();
	}
	if (Mouse::Get()->Press(0))
	{
		pickMousePos[1] = Mouse::Get()->GetPosition();
		frustum->SetPickingArea(pickMousePos[0], pickMousePos[1], box);

		drawArea->ClearBuffer();
		drawArea->AddLine(box[0], box[1]);
		drawArea->AddLine(box[1], box[2]);
		drawArea->AddLine(box[2], box[3]);
		drawArea->AddLine(box[3], box[0]);
		drawArea->UpdateBuffer();
	}
	if (Mouse::Get()->Up(0))
	{
		isMultiPick = true;
		drawArea->ClearBuffer();
	}

	plane->Update();
}

void TestFrustum::Render()
{
	plane->Render();

	drawCount = 0;
	pickCount = 0;
	for (D3DXVECTOR3 position : positions)
	{
		if (frustum->ContainPoint(position))
		{
			sphere->Position(position);
			sphere->SetDiffuse(1, 1, 1);
			if (isMultiPick)
			{
				if (frustum->ContainPoint(position, 1))
				{
					pickCount++;
					sphere->SetDiffuse(0, 0, 1);
				}
			}
			sphere->Update();
			sphere->Render();

			drawCount++;
		}
	}

	depthMode[1]->OMSetDepthStencilState();
	drawArea->Render();
	depthMode[0]->OMSetDepthStencilState();

	ImGui::Begin("TestFrustum");
	{
		ImGui::Text("Draw %d / %d", drawCount, positions.size());
		ImGui::Separator();
		ImGui::Text("Pick %d", pickCount);
	}
	ImGui::End();
}
