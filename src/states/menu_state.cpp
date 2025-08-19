#include "menu_state.h"
#include <game_manager.h>
#include <input_manager.h>
#include <asset_managers/texture_manager.h>

void MenuState::Init(GameManager* gameManager)
{
    this->gameManager = gameManager;
    uiRenderer.Init();

    // Load menu assets
    TextureManager::Get()->Load("PlayButton", "data/textures/play_button.png");
    TextureManager::Get()->Load("ExitButton", "data/textures/exit_button.png");
    TextureManager::Get()->Load("BackGround", "data/textures/blood_1.png");

    // allocate memory for the buttons
    buttons.Init(2, STATIC_MEMORY);

    int width, height;
    PlatformClientDimensions(&width, &height);

    // Create the play button
    UIButton<MenuState> playButton;
    playButton.Init(Vector2(width*0.5f - 150, (height*0.5f - 50) + 60), Vector2(300, 100), "PlayButton", this, &MenuState::OnPlayButtonClick);
    buttons.Push(playButton);

    // Create the exit button
    UIButton<MenuState> exitButton;
    exitButton.Init(Vector2(width * 0.5f - 150, (height*0.5f - 50) - 60), Vector2(300, 100), "ExitButton", this, &MenuState::OnExitButtonClick);
    buttons.Push(exitButton);
}

void MenuState::Terminate()
{
    uiRenderer.Terminate();
}


void MenuState::OnEnter()
{
}

void MenuState::OnExit()
{
}

void MenuState::OnUpdate(float deltaTime)
{
    for (int i = 0; i < buttons.size; i++)
    {
        buttons[i].Update();
    }
}

void MenuState::OnRender()
{
    // Render Background
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            int width = 512;
            int height = 512;
            Vector2 size = Vector2(width, height);
            Vector2 position = Vector2(width * j, height * i) + size * 0.5f;
            uiRenderer.DrawQuat(position, size, 99, "BackGround");
        }
    }

    // Render Buttons
    for (int i = 0; i < buttons.size; i++)
    {
        buttons[i].Render(uiRenderer, 0);
    }
}

void MenuState::OnPlayButtonClick()
{
    gameManager->ChangeToGameState();
}

void MenuState::OnExitButtonClick()
{
    PlatformQuitApplication();
}