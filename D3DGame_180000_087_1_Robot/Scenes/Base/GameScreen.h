#pragma once

class GameScreen
{
public:
	enum class ScreenState
	{
		TransitionOn,
		Active,
		TransitionOff,
		Hidden,
		Finished,
	};

public:
	GameScreen()
		: isPopup(false), isExiting(false)
		, transitionPosition(1.0f)
		, screenState(ScreenState::TransitionOn)
	{}

	virtual ~GameScreen() {}
	virtual void LoadContent() {}
	virtual void UnloadContent() {}
	virtual void InitScreen() {}
	virtual void FinalScreen() {}
	virtual void Update(bool otherScreenHasFocus, bool coveredByOtherScreen);
	virtual void Render() = 0;
	void ExitScreen();

public:
	bool IsPopup() { return isPopup; }
	float GetTransitionOnTime() { return transitionOnTime; }
	float GetTransitionOffTime() { return transitionOffTime; }
	float GetTransitionPosition() { return transitionPosition; }
	unsigned char GetTransitionAlpha() { return (unsigned char)(255 - transitionPosition * 255); }
	ScreenState GetScreenState() { return screenState; }
	bool IsExiting() { return isExiting; }
	bool IsActive() {
		return !otherScreenHasFocus &&
			(screenState == ScreenState::TransitionOn || screenState == ScreenState::Active);
	}

public:
	GameScreen* nextScreen;
	class GameScreenManager* gameScreenManager;
	

protected:
	bool isPopup;
	float transitionOnTime;
	float transitionOffTime;
	float transitionPosition;
	
	ScreenState screenState;

	bool isExiting;
	bool otherScreenHasFocus;

private:
	bool UpdateTransition(float time, int direction);
};