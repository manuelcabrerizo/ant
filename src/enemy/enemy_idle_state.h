#pragma once

#include <states/state.h>

class EnemyComponent;

class EnemyIdleState : public IState
{
private:
    EnemyComponent* owner = nullptr;
public:
    void Init(EnemyComponent* owner);
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
};