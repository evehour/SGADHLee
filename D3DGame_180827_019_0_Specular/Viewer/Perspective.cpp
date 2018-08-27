#include "stdafx.h"
#include "Perspective.h"

Perspective::Perspective(float width, float height, float fov, float zn, float zf)
	: Projection(zn, zf)
{
	Set(width, height, fov, zn, zf);
}

Perspective::~Perspective()
{
}

void Perspective::Set(float width, float height, float fov, float zn, float zf)
{
	this->width = width;
	this->height = height;
	this->fov = fov;
	this->aspect = width / height;
	this->zNear = zn;
	this->zFar = zf;

	D3DXMatrixPerspectiveFovLH(&matrix, fov, aspect, zn, zf);
}
