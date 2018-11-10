#pragma once

class GameTool : public Execute
{
public:
	GameTool(ExecuteValues* values);
	~GameTool();

	void Update();
	void PreRender() {};
	void Render() {};
	void PostRender() {};
	void ResizeScreen() {};

private:
	ExecuteValues* values;

};