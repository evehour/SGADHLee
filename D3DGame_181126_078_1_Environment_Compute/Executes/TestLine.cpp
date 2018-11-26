#include "stdafx.h"
#include "TestLine.h"


TestLine::TestLine()
{
	bBox = new BBox();
	((BBox*)bBox)->Min = D3DXVECTOR3(0, 0, 0);
	((BBox*)bBox)->Min = D3DXVECTOR3(1, 1, 1);
	boxLine = new DebugLine();

	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);
	boxLine->Color(1, 0, 0);
	boxLine->Draw(W, bBox);

	cubeMaterial = new Material(Shaders + L"052_Mesh.fx");
	cubeMaterial->SetDiffuseMap(Textures + L"Floor.png");
	cube = new MeshCube(cubeMaterial, 1, 1, 1);
	cube->Position(0.5f, 0.5f, 0.5f);
	
}

TestLine::~TestLine()
{
	SAFE_DELETE(bBox);
	SAFE_DELETE(boxLine);

	SAFE_DELETE(cube);
	SAFE_DELETE(cubeMaterial);
}

void TestLine::Update()
{
	
}

void TestLine::PreRender()
{

}

void TestLine::Render()
{
	boxLine->Render();
	cube->Render();
}

void TestLine::PostRender()
{

}


