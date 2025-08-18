#pragma once

#include "state.h"

class MenuState : public IState
{
public:
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
};