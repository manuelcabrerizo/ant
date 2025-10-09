#pragma once

#include "state.h"
#include <stack>

class StateMachine
{
private:
    std::stack<IState*> states;
public:
    void Push(IState* state);
    void Pop();
    void ChangeState(IState* state);
    IState* Peek();
    void Update(float deltaTime);
    void Render();
    void Clear();
};