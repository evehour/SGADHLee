#pragma once

class Projection
{
public:
	Projection(float zNear, float zFar);
	virtual ~Projection();

	void Matrix(D3DXMATRIX* matrix);

protected:
	float zNear, zFar;

	D3DXMATRIX matrix;
};