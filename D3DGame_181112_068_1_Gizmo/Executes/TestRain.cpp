#include "stdafx.h"
#include "TestRain.h"
#include "../Landscape/NRain.h"


TestRain::TestRain(ExecuteValues * values)
	:Execute(values)
{
	rain = new NRain(values);
}

TestRain::~TestRain()
{
	SAFE_DELETE(rain);
}

void TestRain::Update()
{
	rain->Update();
}

void TestRain::PreRender()
{

}

void TestRain::Render()
{
	rain->Render();
}

void TestRain::PostRender()
{

}


