#include "stdafx.h"
#include "Shape.h"
#include "../Model/Model.h"
#include "../Model/ModelBone.h"
#include "../Model/ModelMesh.h"

Shape::Shape(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);	
}

Shape::~Shape()
{
}

void Shape::SetShader(wstring file)
{
}

void Shape::Update()
{
}

void Shape::Render()
{
}
