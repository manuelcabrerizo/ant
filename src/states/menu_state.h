#pragma once

#include "state.h"

class GameManager;

class MenuState : public IState
{
private:
    GameManager* gameManager = nullptr;
public:
    void Init(GameManager* gameManager);
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
};