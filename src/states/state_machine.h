#pragma once

#include "state.h"

class StateMachine
{
private:
    // NOTE: for now we are going to have just one
    // State at a time, late this should be a Stack container
    // so we can handle more than one state at a time
    IState* current = nullptr;
public:
    void Push(IState* state);
    void Pop();
    void ChangeState(IState* state);
    void Update(float deltaTime);
    void Render();
    void Clear();
};