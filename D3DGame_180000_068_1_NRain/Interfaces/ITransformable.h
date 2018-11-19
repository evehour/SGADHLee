#pragma once
class ITransformable
{
public:
	ITransformable() {}
	~ITransformable() {}
	
	virtual bool Intersect(struct Ray* ray, OUT float& result) = 0;
};