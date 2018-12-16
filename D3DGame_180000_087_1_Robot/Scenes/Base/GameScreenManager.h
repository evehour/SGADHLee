#pragma once

class GameScreen;
class GameRender;

class GameScreenManager
{
public:
	GameScreenManager();
	~GameScreenManager();

	void Update();
	void Render();

protected:
	void LoadContent();
	void UnloadContent();

public:
	void AddScreen(GameScreen* screen, bool callLoadContent);
	void RemoveScreen(GameScreen* screen);
	GameScreen* GetCurrentScreen() { return currentScreen; }
	GameScreen** GetScreens() { return &screens[0]; }

private:
	vector<GameScreen* > screens;
	vector<GameScreen* > screensToUpdate;

	GameScreen* currentScreen;

	Texture* fadeSprite;
	Render2D* fadeObject;
	//TODO: fade object
	/*GameSprite2D fadeSprite = null;
	Sprite2DObject fadeObject = null;*/
};