#include "stdafx.h"
#include "GameScreenManager.h"
#include "GameScreen.h"

#include "./Viewer/Viewport.h"

GameScreenManager::GameScreenManager()
{
	fadeSprite = new Texture(Textures + L"blank.png");
	fadeObject = new Render2D();
	fadeObject->SRV(fadeSprite->SRV());
	fadeObject->Scale(Context::Get()->GetViewport()->GetWidth(), Context::Get()->GetViewport()->GetHeight());
	fadeObject->Visible(false);
}

GameScreenManager::~GameScreenManager()
{
	SAFE_DELETE(fadeObject);
	SAFE_DELETE(fadeSprite);
}

void GameScreenManager::Update()
{
	screensToUpdate.clear();
	for (GameScreen* screen : screens)
		screensToUpdate.push_back(screen);

	// TODO: 창 포커스가 게임인지 아닌지 판단하는 루틴 필요
	// TODO: 창이 다른 창에 의해 덮어져있는지 판단하는 루틴 필요
	bool otherScreenHasFocus = true;
	bool coveredByOtherScreen = false;

	while (screensToUpdate.size() > 0)
	{
		GameScreen* screen = screensToUpdate.back();
		screensToUpdate.pop_back();
		
		screen->Update(otherScreenHasFocus, coveredByOtherScreen);

		//if (screen->GetScreenState() == GameScreen::ScreenState::TransitionOn ||
		//	screen->GetScreenState() == GameScreen::ScreenState::Active)
		//{

		//}
	}
}

void GameScreenManager::Render()
{
	for (GameScreen* screen : screens)
	{
		if (screen->GetScreenState() == GameScreen::ScreenState::Hidden)
			continue;

		screen->Render();
	}
}

void GameScreenManager::AddScreen(GameScreen * screen, bool callLoadContent)
{
	screen->gameScreenManager = this;
	if (callLoadContent)
		screen->LoadContent();

	screen->InitScreen();

	screens.push_back(screen);
	
	currentScreen = screen;
}

void GameScreenManager::RemoveScreen(GameScreen * screen)
{
	screen->FinalScreen();

	screen->UnloadContent();

	if (currentScreen == screen)
		currentScreen = NULL;

	vector<GameScreen*>::iterator it = screens.begin();
	for (;it != screens.end();++it)
	{
		if (*it == screen)
		{
			screens.erase(it);
			break;
		}
	}

	it = screensToUpdate.begin();
	for (;it != screensToUpdate.end();++it)
	{
		if (*it == screen)
		{
			screensToUpdate.erase(it);
			break;
		}
	}
}

void GameScreenManager::LoadContent()
{
	for (GameScreen* screen : screens)
	{
		screen->LoadContent();
	}
}

void GameScreenManager::UnloadContent()
{
	for (GameScreen* screen : screens)
	{
		screen->UnloadContent();
	}
}
