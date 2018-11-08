#pragma once
class Reflection
{
public:
	// 투영영역
	//const float Width = 2048;
	//const float Height = 2048;

public:
	Reflection(ExecuteValues* values);
	~Reflection();

	void Add(class GameModel* obj);

	void Update();
	void PreRender();
	void Render();

private:
	ExecuteValues* values;

	Shader* shader;
	RenderTarget* refectionTarget;
	vector<class GameModel*> objs;

	class Fixity* fixity;
	class Perspective* perspective;
	ViewProjectionBuffer* vpBuffer;
};