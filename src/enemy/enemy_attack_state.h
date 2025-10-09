#pragma once

#include <states/state.h>


class EnemyComponent;
class AnimationNode;

class EnemyAttackState : public IState
{
private:
    EnemyComponent* owner = nullptr;
    AnimationNode* animationNode = nullptr;
    bool isPlayerHit = false;
    void TryAttack();
public:
    void Init(EnemyComponent* owner);
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
};