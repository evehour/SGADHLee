#pragma once

class IBoundable
{
public:
	virtual bool Intersect(struct Ray* ray, float & result) = 0;
};