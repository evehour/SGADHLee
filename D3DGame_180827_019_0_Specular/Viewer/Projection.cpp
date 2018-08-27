#include "stdafx.h"
#include "Projection.h"

Projection::Projection(float zNear, float zFar)
	: zNear(zNear), zFar(zFar)
{

}

Projection::~Projection()
{

}

void Projection::Matrix(D3DXMATRIX * matrix)
{
	*matrix = this->matrix;
}
