#include "stdafx.h"
#include "Orthographic.h"

Orthographic::Orthographic(float left, float right, float bottom, float top, float zn, float zf)
	: Projection(zn, zf)
{
	Set(left, right, bottom, top, zn, zf);
}

Orthographic::~Orthographic()
{

}

void Orthographic::Set(float left, float right, float bottom, float top, float zn, float zf)
{
	this->left = left;
	this->right = right;
	this->bottom = bottom;
	this->top = top;
	this->zNear = zn;
	this->zFar = zf;

	D3DXMatrixOrthoOffCenterLH(&matrix, left, right, bottom, top, zn, zf);
}
