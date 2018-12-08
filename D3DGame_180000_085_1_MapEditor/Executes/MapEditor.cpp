#include "stdafx.h"
#include "MapEditor.h"

#include "../Model/Model.h"
#include "./Model/ModelMesh.h"
#include "../Objects/GameAnimModel.h"

MapEditor::MapEditor()
{
	ma = new GameAnimModel(Shaders + L"052_Model.fx",
		Materials + L"Meshes/", L"Quad.material",
		Models + L"Meshes/", L"Quad.mesh"
	);

	ma->SetDiffuseMap(Textures + L"Dirt.png");
	ma->Scale(1, 1, 1);
	for (ModelMesh* mesh : ma->GetModel()->Meshes())
	{
		mesh->Pass(0);
	}
}

MapEditor::~MapEditor()
{
	SAFE_DELETE(ma);
}

void MapEditor::Update()
{
	ma->Update();
}

void MapEditor::PreRender()
{
}

void MapEditor::Render()
{
	ma->Render();
}

void MapEditor::PostRender()
{
}

void MapEditor::PostRender2D()
{
}
