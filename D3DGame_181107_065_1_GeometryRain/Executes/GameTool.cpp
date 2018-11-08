#include "stdafx.h"
#include "GameTool.h"

void ABClass::SetAB(int a, int b)
{
	this->a = a;
	this->b = b;
}

void ABClass::GetAB(int & a, int & b)
{
	a = this->a;
	b = this->b;
}

int ABClass::aaa(int a)
{
	this->a = a;
	return 0;
}

int ABClass::bbb(int b)
{
	this->b = b;
	return 0;
}

GameTool::GameTool(ExecuteValues* values)
	: Execute(values)
{
	abClass = new ABClass();
	abClass->SetAB(5, 3);

	IAClass* k1 = dynamic_cast<IAClass*>(abClass);
	IBClass* k2 = dynamic_cast<IBClass*>(abClass);
	//IBClass* k2 = dynamic_cast<IBClass*>(k1);

	k1->aaa(20);
	k2->bbb(105);

	int c, d;
	abClass->GetAB(c, d);

	int odd = 20;
	odd = odd;
}

GameTool::~GameTool()
{
}

void GameTool::Update()
{
}
