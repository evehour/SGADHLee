#pragma once

class IBoundable
{
public:
	IBoundable() {}
	virtual ~IBoundable() {}

	virtual bool Intersect(struct Ray* ray, OUT float & result) = 0;
};