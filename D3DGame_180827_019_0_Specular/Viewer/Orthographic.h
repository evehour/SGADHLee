#pragma once
#include "Projection.h"

class Orthographic : public Projection
{
public:
	Orthographic(float left, float right, float bottom, float top, float zn = 0.0f, float zf = 1.0f);
	~Orthographic();

	void Set
	(
		float left, float right, float bottom, float top
		, float zn = 0.0f, float zf = 1.0f
	);

private:
	float left, right;
	float bottom, top;
};