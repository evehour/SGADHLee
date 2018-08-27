#pragma once

class Shape : public Renderer
{
public:
	Shape(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile);
	~Shape();

	void SetShader(wstring file);

	void Update();
	void Render();

protected:
	class Model* model;
};