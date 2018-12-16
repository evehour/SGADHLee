#include "stdafx.h"
#include "GameScreen.h"
#include "GameScreenManager.h"

inline void GameScreen::Update(bool otherScreenHasFocus, bool coveredByOtherScreen)
{
	this->otherScreenHasFocus = otherScreenHasFocus;

	if (isExiting)
	{
		screenState = ScreenState::TransitionOff;
		if (!UpdateTransition(transitionOffTime, 1))
		{
			isExiting = false;
			screenState = ScreenState::Finished;
		}
	}
}

inline void GameScreen::ExitScreen()
{
	if (transitionOffTime <= Math::EPSILON)
	{
		this->gameScreenManager->RemoveScreen(this);
	}
	else
	{
		isExiting = true;
	}
}

inline bool GameScreen::UpdateTransition(float time, int direction)
{
	// How much should we move by?
	float transitionDelta;

	if (time <= Math::EPSILON)
		transitionDelta = 1.0f;
	else
		transitionDelta = Time::Delta();

	transitionPosition += transitionDelta * direction;

	if ((transitionPosition <= 0.0f) || (transitionPosition >= 1.0f))
	{
		transitionPosition = Math::Clamp(transitionPosition, 0.0f, 1.0f);
		return false;
	}

	return true;
}
