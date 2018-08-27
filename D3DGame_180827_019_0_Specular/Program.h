#pragma once

class Program
{
public:
	Program();
	~Program();

	void Update();

	void PreRender();
	void Render();

	void Resize();

private:
	vector<class Execute *> executes;
};