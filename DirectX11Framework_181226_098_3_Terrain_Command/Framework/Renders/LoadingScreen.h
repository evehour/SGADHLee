#pragma once

class LoadingScreen
{
public:
	static LoadingScreen* Get();

	static void Create();
	static void Delete();

	void Initialize();
	void Update(float dt);
	void Render();

	static bool IsProgressing() { return bIncreasing; }
	static bool IsCompleted() { return bComplete; }

	void SetIncRateValue(float val);

	void AddPercent(float val);
	void AdjPercent(float val);
	void SetPercent(float val);

private:
	LoadingScreen();
	~LoadingScreen();

private:
	static LoadingScreen* instance;

	// 증가 비율 (Default: 500)
	float incRateValue;

	float currentPercentage;
	float destPercent;

	vector<float> incStorage;

	static bool bIncreasing, bComplete;
};

#define Loading LoadingScreen::Get()