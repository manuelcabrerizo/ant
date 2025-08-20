#pragma once

#include "state.h"
#include <containers.h>
#include <ui_renderer.h>
#include <ui_button.h>
#include <notification_manager.h>

class GameManager;

class MenuState : public IState, INotificable
{
private:
    GameManager* gameManager = nullptr;
    UIRenderer uiRenderer;
    Array<UIButton<MenuState>> buttons;
    int windowWidth;
    int windowHeight;
public:
    void Init(GameManager* gameManager);
    void Terminate();


    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnResize(OnResizeNotification* onResize);
    void OnNotify(NotificationType type, Notification* notification) override;

    void OnPlayButtonClick();
    void OnExitButtonClick();
    void DrawBackGround();
};