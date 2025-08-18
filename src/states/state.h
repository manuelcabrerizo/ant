#pragma once

class IState
{
public:
    virtual void OnEnter() {};
    virtual void OnExit() {};
    virtual void OnUpdate(float deltaTime) {};
    virtual void OnRender() {};
};