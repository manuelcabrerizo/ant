#include "state_machine.h"

void StateMachine::Push(IState* state)
{
    if (states.empty() || states.top() != state)
    {
        state->OnEnter();
        states.push(state);
    }
}

void StateMachine::Pop()
{
    if (!states.empty())
    {
        states.top()->OnExit();
        states.pop();
    }
}

void StateMachine::ChangeState(IState* state)
{
    Clear();
    Push(state);
}

IState* StateMachine::Peek()
{
    return !states.empty() ? states.top() : nullptr;
}

void StateMachine::Update(float deltaTime)
{
    if (!states.empty())
    {
        states.top()->OnUpdate(deltaTime);
    }
}

void StateMachine::Render()
{
    if (!states.empty())
    {
        states.top()->OnRender();
    }
}

void StateMachine::Clear()
{
    while (!states.empty())
    {
        Pop();
    }
}