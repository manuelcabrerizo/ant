#include "menu_state.h"
#include <game_manager.h>
#include <input_manager.h>
#include <asset_managers/texture_manager.h>
#include <notification_manager.h>

void MenuState::Init(GameManager* gameManager)
{
    this->gameManager = gameManager;
    uiRenderer.Init();

    // Load menu assets
    TextureManager::Get()->Load("PlayButton", "data/textures/play_button.png", STATIC_MEMORY);
    TextureManager::Get()->Load("PlayButtonSelected", "data/textures/play_button_selected.png", STATIC_MEMORY);
    TextureManager::Get()->Load("PlayButtonClicked", "data/textures/play_button_clicked.png", STATIC_MEMORY);
    TextureManager::Get()->Load("ExitButton", "data/textures/exit_button.png", STATIC_MEMORY);
    TextureManager::Get()->Load("ExitButtonSelected", "data/textures/exit_button_selected.png", STATIC_MEMORY);
    TextureManager::Get()->Load("ExitButtonClicked", "data/textures/exit_button_clicked.png", STATIC_MEMORY);
    TextureManager::Get()->Load("BackGround", "data/textures/blood_1.png", STATIC_MEMORY);

    // allocate memory for the buttons
    buttons.Init(2, STATIC_MEMORY);

    PlatformClientDimensions(&windowWidth, &windowHeight);

    Vector2 extent = Vector2(windowWidth, windowHeight);
    Vector2 size = Vector2(300, 100);
    Vector2 position = (extent * 0.5f) - (size * 0.5f);
    Vector2 offset = Vector2(0.0f, 60.0f);
    
    // Create the play button
    UIButton<MenuState> playButton;
    const char* playButtonTextures[] = { "PlayButton", "PlayButtonSelected", "PlayButtonClicked" };
    playButton.Init(position + offset, size, playButtonTextures, this, &MenuState::OnPlayButtonClick);
    buttons.Push(playButton);

    // Create the exit button
    UIButton<MenuState> exitButton;
    const char* exitButtonTextures[] = { "ExitButton", "ExitButtonSelected", "ExitButtonClicked" };
    exitButton.Init(position - offset, size, exitButtonTextures, this, &MenuState::OnExitButtonClick);
    buttons.Push(exitButton);
}

void MenuState::Terminate()
{
    uiRenderer.Terminate();
}

void MenuState::OnEnter()
{
    NotificationManager::Get()->AddListener(this, NotificationType::OnResize);

    PlatformClientDimensions(&windowWidth, &windowHeight);
    OnResizeNotification notification;
    notification.extent = Vector2(windowWidth, windowHeight);
    OnResize(&notification);
    PlatformShowMouse(true);

    // set render state
    GraphicsManager::Get()->SetBlendingOff();
    GraphicsManager::Get()->SetDepthStencilOn();

}

void MenuState::OnExit()
{
    PlatformShowMouse(false);

    NotificationManager::Get()->RemoveListener(this, NotificationType::OnResize);
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
    DrawBackGround();
    for (int i = 0; i < buttons.size; i++)
    {
        buttons[i].Render(uiRenderer, 0);
    }
}

void MenuState::OnResize(OnResizeNotification* onResize)
{
    windowWidth = onResize->extent.x;
    windowHeight = onResize->extent.y;
    Vector2 extent = onResize->extent;
    Vector2 size = Vector2(300, 100);
    Vector2 position = (extent * 0.5f) - (size * 0.5f);
    Vector2 offset = Vector2(0.0f, 60.0f);
    buttons[0].SetPosition(position + offset);
    buttons[1].SetPosition(position - offset);
    uiRenderer.OnResize(extent);
}

void MenuState::OnPlayButtonClick()
{
    gameManager->ChangeToPlayState();
}

void MenuState::OnExitButtonClick()
{
    PlatformQuitApplication();
}

void MenuState::DrawBackGround()
{
    int tileWidth = 512;
    int tileHeight = 512;
    int rowCount = (int)ceilf((float)windowHeight / (float)tileHeight);
    int colCount = (int)ceilf((float)windowWidth / (float)tileWidth);
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < colCount; j++)
        {
            Vector2 size = Vector2(tileWidth, tileHeight);
            Vector2 position = Vector2(tileWidth * j, tileHeight * i) + size * 0.5f;
            uiRenderer.DrawQuat(position, size, 99, "BackGround");
        }
    }
}

void MenuState::OnNotify(NotificationType type, Notification* notification)
{
    switch (type)
    {
        case NotificationType::OnResize: OnResize((OnResizeNotification*)notification); break;
    }
}