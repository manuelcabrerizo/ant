#include "enemy_walk_state.h"
#include <components/enemy_component.h>
#include <components/transform_component.h>
#include <components/physics_component.h>
#include <components/animation_component.h>

#include <math/algebra.h>

void EnemyWalkState::Init(EnemyComponent* owner)
{
    this->owner = owner;

    owner->pursue.SetCharacter(&owner->character);
    owner->pursue.SetTarget(&owner->target);
    owner->pursue.SetMaxAcceleration(7.5f);
    owner->pursue.SetMaxSpeed(10.0f);
    owner->pursue.SetTargetRadius(ANT_PI * 0.1f);
    owner->pursue.SetSlowRadius(ANT_PI * 0.5f);
    owner->pursue.SetTimeToTarget(0.5f);
    owner->pursue.SetMaxPrediction(0.1f);

    owner->face.SetCharacter(&owner->character);
    owner->face.SetTarget(&owner->target);
    owner->face.SetMaxAngularAcceleration(8.0f);
    owner->face.SetMaxRotation(4.0f);
    owner->face.SetTargetRadius(ANT_PI * 0.1f);
    owner->face.SetSlowRadius(ANT_PI * 0.5f);
    owner->face.SetTimeToTarget(0.01f);
}

void EnemyWalkState::OnEnter()
{
    owner->animation->Transition((int)EnemyAnimation::Walk, 0.5f);
}

void EnemyWalkState::OnExit()
{
}

void EnemyWalkState::OnUpdate(float deltaTime)
{
    SteeringOutput lookSeering = owner->face.GetSteering();
    owner->character.rotation += lookSeering.angular * deltaTime;
    owner->character.rotation *= powf(0.001f, deltaTime); // angular drag
    owner->character.orientation += owner->character.rotation * deltaTime;
    owner->transform->rotation.y = owner->character.orientation * -1.0f;

    Vector3 moveDir = Vector3(
       -sinf(owner->character.orientation),
        0.0f,
        cosf(owner->character.orientation)).Normalized();

    SteeringOutput pursueSteering = owner->pursue.GetSteering();
    owner->physics->acceleration = moveDir * pursueSteering.linear.Magnitude();
    owner->character.position = owner->transform->position;
    owner->character.velocity = owner->physics->velocity;
    owner->character.velocity.y = 0.0f;

    float distToTargetSq = (owner->transform->position - owner->target.position).MagnitudeSq();
    if (distToTargetSq <= owner->attackRadio * owner->attackRadio)
    {
        owner->states.ChangeState(&owner->attackState);
    }
    if (distToTargetSq > owner->detectionRadio * owner->detectionRadio)
    {
        owner->states.ChangeState(&owner->idleState);
    }
}