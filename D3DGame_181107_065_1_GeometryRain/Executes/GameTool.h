#pragma once

class IAClass
{
public:
	IAClass() {};
	virtual ~IAClass() {};

	virtual int aaa(int a) = 0;
};

class IBClass
{
public:
	IBClass() {};
	virtual ~IBClass() {};

	virtual int bbb(int b) = 0;
};

class ABClass : public IAClass, public IBClass
{
public:
	ABClass() {};
	~ABClass() {};

	void SetAB(int a, int b);
	void GetAB(int& a, int& b);

	int aaa(int a) override;
	int bbb(int b) override;

private:
	int a, b;
};

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
	ABClass* abClass;
};