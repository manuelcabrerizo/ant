#pragma once

#include <states/state.h>

class EnemyComponent;

class EnemyHitState : public IState
{
private:
    EnemyComponent* owner = nullptr;
    float timeToWait = 0.75f;
    float timer = 0;
public:
    void Init(EnemyComponent* owner);
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
};