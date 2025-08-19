#pragma once

#include "state.h"
#include <containers.h>
#include <ui_renderer.h>
#include <ui_button.h>

class GameManager;

class MenuState : public IState
{
private:
    GameManager* gameManager = nullptr;
    UIRenderer uiRenderer;
    Array<UIButton<MenuState>> buttons;
public:
    void Init(GameManager* gameManager);
    void Terminate();


    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;

    void OnPlayButtonClick();
    void OnExitButtonClick();
};