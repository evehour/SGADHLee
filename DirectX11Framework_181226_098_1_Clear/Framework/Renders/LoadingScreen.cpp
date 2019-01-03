#include "Framework.h"
#include "LoadingScreen.h"

LoadingScreen* LoadingScreen::instance = NULL;
bool LoadingScreen::bIncreasing = false;
bool LoadingScreen::bComplete = false;

LoadingScreen * LoadingScreen::Get()
{
	assert(instance != NULL);

	return instance;
}

void LoadingScreen::Create()
{
	assert(instance == NULL);

	instance = new LoadingScreen();
}

void LoadingScreen::Delete()
{
	SAFE_DELETE(instance);
}

void LoadingScreen::Initialize()
{
	incRateValue = 500.0;
	currentPercentage = destPercent = 0.0f;
	incStorage.clear();

	bIncreasing = bComplete = false;
}

void LoadingScreen::Update(float dt)
{
	if (bComplete == true)
		return;

	if (bIncreasing == true)
	{
		currentPercentage += incRateValue * dt;

		if (destPercent <= currentPercentage)
		{
			currentPercentage = destPercent;
			bIncreasing = false;
		}
	}
	else if (incStorage.size() > 0)
	{
		bIncreasing = true;
		destPercent = currentPercentage + incStorage.back();
		incStorage.pop_back();
	}

	if (currentPercentage >= 100)
	{
		currentPercentage = 100;

		bComplete = true;
		bIncreasing = false;
	}
}

void LoadingScreen::Render()
{
	ImGui::ProgressBar(currentPercentage / 100.0f);
}

void LoadingScreen::SetIncRateValue(float val)
{
	incRateValue = val;
}

void LoadingScreen::AddPercent(float val)
{
	incStorage.push_back(val);
}

void LoadingScreen::AdjPercent(float val)
{
	if (bIncreasing)
	{
		destPercent = val;
	}
	else
	{
		AddPercent(val - currentPercentage);
	}
}

void LoadingScreen::SetPercent(float val)
{
	currentPercentage = val;
	if (bIncreasing == true)
		bIncreasing = false;
}

LoadingScreen::LoadingScreen()
{
	Initialize();
}

LoadingScreen::~LoadingScreen()
{
}