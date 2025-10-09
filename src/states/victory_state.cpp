#include "victory_state.h"
#include <game_manager.h>
#include <input_manager.h>
#include <asset_managers/texture_manager.h>
#include <notification_manager.h>

void VictoryState::Init(GameManager* gameManager)
{
    this->gameManager = gameManager;
    uiRenderer.Init();
    textRenderer.Init("data/fonts/times.rtpa", STATIC_MEMORY);

    // Load menu assets
    TextureManager::Get()->Load("PlayButton", "data/textures/play_button.png", STATIC_MEMORY, false);
    TextureManager::Get()->Load("PlayButtonSelected", "data/textures/play_button_selected.png", STATIC_MEMORY, false);
    TextureManager::Get()->Load("PlayButtonClicked", "data/textures/play_button_clicked.png", STATIC_MEMORY, false);
    TextureManager::Get()->Load("MenuButton", "data/textures/menu_button.png", STATIC_MEMORY, false);
    TextureManager::Get()->Load("MenuButtonSelected", "data/textures/menu_button_selected.png", STATIC_MEMORY, false);
    TextureManager::Get()->Load("MenuButtonClicked", "data/textures/menu_button_clicked.png", STATIC_MEMORY, false);
    TextureManager::Get()->Load("PauseBackground", "data/textures/stone_hell_5.png", STATIC_MEMORY, false);

    // allocate memory for the buttons
    buttons.Init(2, STATIC_MEMORY);

    PlatformClientDimensions(&windowWidth, &windowHeight);

    Vector2 extent = Vector2(windowWidth, windowHeight);
    Vector2 size = Vector2(300, 100);
    Vector2 position = (extent * 0.5f) - (size * 0.5f);
    Vector2 offset = Vector2(0.0f, 60.0f);
    
    // Create the play button
    UIButton<VictoryState> playButton;
    const char* playButtonTextures[] = { "PlayButton", "PlayButtonSelected", "PlayButtonClicked" };
    playButton.Init(position + offset, size, playButtonTextures, this, &VictoryState::OnPlayButtonClick);
    buttons.Push(playButton);

    // Create the exit button
    UIButton<VictoryState> exitButton;
    const char* exitButtonTextures[] = { "MenuButton", "MenuButtonSelected", "MenuButtonClicked" };
    exitButton.Init(position - offset, size, exitButtonTextures, this, &VictoryState::OnMenuButtonClick);
    buttons.Push(exitButton);
}

void VictoryState::Terminate()
{
    textRenderer.Terminate();
    uiRenderer.Terminate();
}

void VictoryState::OnEnter()
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

void VictoryState::OnExit()
{
    PlatformShowMouse(false);

    NotificationManager::Get()->RemoveListener(this, NotificationType::OnResize);
}

void VictoryState::OnUpdate(float deltaTime)
{
    for (int i = 0; i < buttons.size; i++)
    {
        buttons[i].Update();
    }
}

void VictoryState::OnRender()
{
    GraphicsManager::Get()->BackBufferBind();
    GraphicsManager::Get()->BeginFrame(0, 0, 0);

    DrawBackGround();
    for (int i = 0; i < buttons.size; i++)
    {
        buttons[i].Render(uiRenderer, 0);
    }

    GraphicsManager::Get()->SetAlphaBlending();
    GraphicsManager::Get()->SetDepthStencilOnWriteMaskZero();
    GraphicsManager::Get()->SetRasterizerStateCullNone();

    float ratioX = windowWidth / 1920.0f;
    float ratioY = windowHeight / 1080.0f;
    float positionX = (windowWidth * 0.5f) - (375 * ratioX);
    float positionY = windowHeight - (300 * ratioY);
    textRenderer.DrawString("VICTORY", Vector2(positionX, positionY), 6.0f*ratioX, Vector3(0.5, 1.0f, 0.5f));

    GraphicsManager::Get()->SetRasterizerStateCullBack();
    GraphicsManager::Get()->SetDepthStencilOn();
    GraphicsManager::Get()->EndFrame(1);
}

void VictoryState::OnResize(OnResizeNotification* onResize)
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
    textRenderer.OnResize(extent);
}

void VictoryState::OnPlayButtonClick()
{
    gameManager->ChangeToPlayState();
}

void VictoryState::OnMenuButtonClick()
{
    gameManager->ChangeToMenuState();
}

void VictoryState::DrawBackGround()
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
            uiRenderer.DrawQuat(position, size, 99, "PauseBackground");
        }
    }
}

void VictoryState::OnNotify(NotificationType type, Notification* notification)
{
    switch (type)
    {
        case NotificationType::OnResize: OnResize((OnResizeNotification*)notification); break;
    }
}