#include "enemy_hit_state.h"
#include <components/enemy_component.h>
#include <components/animation_component.h>

void EnemyHitState::Init(EnemyComponent* owner)
{
    this->owner = owner;
}

void EnemyHitState::OnEnter()
{
    owner->animation->Transition((int)EnemyAnimation::Hit, 0.25f);
    timer = timeToWait;
}

void EnemyHitState::OnExit()
{
}

void EnemyHitState::OnUpdate(float deltaTime)
{
    if (timer <= 0.0f)
    {
        owner->states.ChangeState(&owner->idleState);
    }
    timer -= deltaTime;
}