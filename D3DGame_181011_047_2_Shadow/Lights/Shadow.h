#pragma once
class Shadow
{
public:
	// 투영영역
	//const float Width = 2048;
	//const float Height = 2048;

public:
	Shadow(ExecuteValues* values);
	~Shadow();

	void Add(class GameModel* obj);

	void Update();
	void PreRender();
	void Render();

private:
	ExecuteValues* values;

	Shader* shaderDepth;
	RenderTarget* depthShadow;
	vector<class GameModel*> objs;
	class Render2D* depthRender;

	class Fixity* fixity;
	class Perspective* perspective;
	ViewProjectionBuffer* vpBuffer;
};