#include "stdafx.h"
#include "Time.h"

Time* Time::instance = NULL;

bool Time::isTimerStopped = true;
float Time::timeElapsed = 0.0f;

Time::Time(void) :
	ticksPerSecond(0), currentTime(0), lastTime(0), lastFPSUpdate(0), fpsUpdateInterval(0),
	frameCount(0), runningTime(0), framePerSecond(0)
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond);
	fpsUpdateInterval = ticksPerSecond >> 1;

	/*TwBar* bar = TweakBar::Get()->GetBar();
	TwAddVarRO(bar, "Time", TW_TYPE_FLOAT, &framePerSecond, "");*/

	invokers = new Invoker[10];
}

Time::~Time(void)
{
	SAFE_DELETE_ARRAY(invokers);
}

void Time::CheckInvoker()
{
	for (UINT i = 0; i < 10; i++)
	{
		if (!invokers[i].bUsed) return;

		if (runningTime >= invokers[i].invokeStartTime)
		{
			invokers[i].invokeFunc();
			invokers[i].bUsed = false;
		}
	}
}

void Time::AddInvoker(float startDeltaTime, function<void(void)> func)
{
	Invoker invoker;
	invoker.bUsed = true;
	invoker.invokeRegistTime = runningTime;
	invoker.invokeStartTime = runningTime + startDeltaTime;
	invoker.invokeFunc = func;

	for (UINT i = 0; i < 10; i++)
	{
		if (!invokers[i].bUsed)
		{
			invokers[i] = invoker;
			break;
		}
	}
}

Time* Time::Get()
{
	assert(instance != NULL);
	
	return instance;
}

void Time::Create()
{
	assert(instance == NULL);

	instance = new Time();
}

void Time::Delete()
{
	SAFE_DELETE(instance);
}

void Time::Update()
{
	if (isTimerStopped) return;

	//1. ����ð��� ������ �ð� ���� �� ���� �ð��� ����Ѵ�.
	QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);
	timeElapsed = (float)(currentTime - lastTime) / (float)ticksPerSecond;
	runningTime += timeElapsed;


	//2. FPS Update
	frameCount++;
	if (currentTime - lastFPSUpdate >= fpsUpdateInterval)
	{
		float tempCurrentTime = (float)currentTime / (float)ticksPerSecond;
		float tempLastTime = (float)lastFPSUpdate / (float)ticksPerSecond;
		framePerSecond = (float)frameCount / (tempCurrentTime - tempLastTime);

		lastFPSUpdate = (INT64)currentTime;
		frameCount = 0;
	}

	lastTime = currentTime;

	CheckInvoker();
}

void Time::Print()
{

}

void Time::Start()
{
	if (!isTimerStopped)
		assert(false);

	QueryPerformanceCounter((LARGE_INTEGER *)&lastTime);
	isTimerStopped = false;
}

void Time::Stop()
{
	if (isTimerStopped)
		assert(false);

	INT64 stopTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&stopTime);
	runningTime += (float)(stopTime - lastTime) / (float)ticksPerSecond;
	isTimerStopped = true;
}