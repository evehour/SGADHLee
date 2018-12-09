#pragma once

class Program
{
public:
	Program();
	~Program();

	void Update();

	void PreRender();
	void Render();
	void PostRender();
	void PostRender2D();

	void ResizeScreen();

private:
	vector<class Execute *> executes;
};