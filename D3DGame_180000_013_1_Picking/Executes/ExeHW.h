#pragma once
#include "Execute.h"

class ExeHW : public Execute
{
public:
	ExeHW(ExecuteValues* values);
	~ExeHW();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	bool IsPick(class MeshObject* object);

private:
	vector<class MeshObject *> objects;
	class MeshObject* pickObj;
	int pickIdx;
};