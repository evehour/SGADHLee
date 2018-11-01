#pragma once
#include "Execute.h"

class TestVolumedCloud : public Execute
{
public:
	TestVolumedCloud(ExecuteValues* values);
	~TestVolumedCloud();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen(){}

private:
	class VolumedCloud* volumedCloud;
};