#include "state_machine.h"

void StateMachine::Push(IState* state)
{
    if (current == nullptr ||
        current != state)
    {
        current = state;
        current->OnEnter();
    }
}

void StateMachine::Pop()
{
    if (current != nullptr)
    {
        current->OnExit();
        current = nullptr;
    }
}

void StateMachine::ChangeState(IState* state)
{
    Pop();
    Push(state);
}

void StateMachine::Update(float deltaTime)
{
    if (current != nullptr)
    {
        current->OnUpdate(deltaTime);
    }
}

void StateMachine::Render()
{
    if (current != nullptr)
    {
        current->OnRender();
    }
}

void StateMachine::Clear()
{
    while (current != nullptr)
    {
        Pop();
    }
}