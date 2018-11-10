#pragma once
class Projection
{
public:
	// 투영영역
	//const float Width = 2048;
	//const float Height = 2048;

public:
	Projection(ExecuteValues* values);
	~Projection();

	void Add(class GameModel* obj);

	void Update();
	void PreRender();
	void Render();

private:
	ExecuteValues* values;

	Shader* shader;
	vector<class GameModel*> objs;

	class Fixity* fixity;
	class Perspective* perspective;
	ViewProjectionBuffer* vpBuffer;
};