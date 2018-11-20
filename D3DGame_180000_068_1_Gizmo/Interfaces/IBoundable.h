#pragma once

class IBoundable
{
public:
	IBoundable() {}
	virtual ~IBoundable() {}

	virtual bool Intersect(struct Ray* ray, float & result) = 0;
};