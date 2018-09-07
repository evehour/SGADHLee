#include "stdafx.h"
#include "TestFrustum.h"
#include "../Viewer/Fixity.h"
#include "../Viewer/Frustum.h"
#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"

TestFrustum::TestFrustum(ExecuteValues * values)
	: Execute(values), drawCount(0)
{
	shader = new Shader(Shaders + L"028_Diffuse.hlsl");

	plane = new MeshPlane();
	plane->Scale(10, 10, 10);
	plane->SetShader(shader);
	plane->SetDiffuse(1, 0, 0, 1);

	sphere = new MeshSphere();
	sphere->SetShader(shader);
	sphere->SetDiffuse(1, 1, 1, 1);

	camera = new Fixity();
	camera->Position(0, 0, -50);

	projection = new Perspective(1024, 768, Math::PI * 0.25f, 1, 1000);

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
}

TestFrustum::~TestFrustum()
{
	
}

void TestFrustum::Update()
{
	frustum->Update();

	plane->Update();
}

void TestFrustum::Render()
{
	plane->Render();

	drawCount = 0;
	for (D3DXVECTOR3 position : positions)
	{
		if (frustum->ContainPoint(position) == true)
		{
			sphere->Position(position);
			
			sphere->Update();
			sphere->Render();

			drawCount++;
		}
	}

	ImGui::Text("Draw %d / %d", drawCount, positions.size());
}

