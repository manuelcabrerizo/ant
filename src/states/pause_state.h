#pragma once

#include "state.h"
#include <containers.h>
#include <ui_renderer.h>
#include <text_renderer.h>
#include <ui_button.h>
#include <notification_manager.h>

class GameManager;

class PauseState : public IState, INotificable
{
private:
    GameManager* gameManager = nullptr;
    UIRenderer uiRenderer;
    TextRenderer textRenderer;

    Array<UIButton<PauseState>> buttons;
    int windowWidth;
    int windowHeight;

    void OnResize(OnResizeNotification* onResize);

public:
    void Init(GameManager* gameManager);
    void Terminate();


    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnNotify(NotificationType type, Notification* notification) override;

    void OnResumeButtonClick();
    void OnMenuButtonClick();
    void DrawBackGround();
};