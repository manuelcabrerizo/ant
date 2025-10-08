#include "enemy_component.h"
#include "transform_component.h"
#include "physics_component.h"
#include "animation_component.h"
#include "collider_component.h"

#include <asset_managers/animation_manager.h>

#include <math.h>
#include <math/algebra.h>

void EnemyComponent::OnInit(ActorManager *actorManager)
{
    NotificationManager::Get()->AddListener(this, NotificationType::EnemyHit);
    NotificationManager::Get()->AddListener(this, NotificationType::PlayerMove);

    transform = owner->GetComponent<TransformComponent>();
    physics = owner->GetComponent<PhysicsComponent>();
    animation = owner->GetComponent<AnimationComponent>();
    collider = owner->GetComponent<ColliderComponent>();

    // Initialize animation component
    animation->SetSkeleton(SkeletonManager::Get()->Get("Bloodwraith"));
    animation->AddAnimation((int)EnemyAnimation::Idle, AnimationManager::Get()->Get("Idle"), false);
    animation->AddAnimation((int)EnemyAnimation::Walk, AnimationManager::Get()->Get("Walking"), true);
    animation->AddAnimation((int)EnemyAnimation::Dead, AnimationManager::Get()->Get("Death"), false);
    animation->SetAnimation((int)EnemyAnimation::Idle);

    pursue.SetCharacter(&character);
    pursue.SetTarget(&target);
    pursue.SetMaxAcceleration(7.5f);
    pursue.SetMaxSpeed(10.0f);
    pursue.SetTargetRadius(ANT_PI * 0.1f);
    pursue.SetSlowRadius(ANT_PI * 0.5f);
    pursue.SetTimeToTarget(0.5f);
    pursue.SetMaxPrediction(0.1f);

    face.SetCharacter(&character);
    face.SetTarget(&target);
    face.SetMaxAngularAcceleration(8.0f);
    face.SetMaxRotation(4.0f);
    face.SetTargetRadius(ANT_PI * 0.1f);
    face.SetSlowRadius(ANT_PI * 0.5f);
    face.SetTimeToTarget(0.01f);

    life = maxLife;
}

void EnemyComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(this, NotificationType::PlayerMove);
    NotificationManager::Get()->RemoveListener(this, NotificationType::EnemyHit);
}

void EnemyComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    float distToTargetSq = (transform->position - target.position).MagnitudeSq();
    wasInRadio = isInRadio;
    isInRadio = (distToTargetSq <= detectionRadio * detectionRadio);
    if (isInRadio)
    {
        if (physics->grounded && life > 0)
        {

            SteeringOutput lookSeering = face.GetSteering();
            character.rotation += lookSeering.angular * dt;
            character.rotation *= powf(0.001f, dt); // angular drag
            character.orientation += character.rotation * dt;
            transform->rotation.y = character.orientation * -1.0f;

            Vector3 moveDir = Vector3(-sinf(character.orientation), 0.0f, cosf(character.orientation)).Normalized();

            SteeringOutput pursueSteering = pursue.GetSteering();
            physics->acceleration = moveDir * pursueSteering.linear.Magnitude();
            character.position = transform->position;
            character.velocity = physics->velocity;
            character.velocity.y = 0.0f;
        }
    }

    if (life > 0 && isInRadio && !wasInRadio)
    {
        animation->Transition((int)EnemyAnimation::Walk, 0.25f);
    }
    if (life > 0 && !isInRadio && wasInRadio)
    {
        animation->Transition((int)EnemyAnimation::Idle, 0.25f);
    }
}

void EnemyComponent::OnEnemyHit(EnemyHitNotification* enemyHit)
{
    if (enemyHit->enemy == owner)
    {
        if ((life - 1) == 0)
        {
            physics->enable = false;
            collider->enable = false;
            animation->Transition((int)EnemyAnimation::Dead, 0.25f);
        }
        life--;
    }
}

void EnemyComponent::OnPlayerMove(PlayerMoveNotification* playerMove)
{
    target.position = playerMove->position;
    target.velocity = playerMove->velocity;
}

void EnemyComponent::OnNotify(NotificationType type, Notification* notification)
{
    switch (type)
    {
    case NotificationType::EnemyHit: OnEnemyHit((EnemyHitNotification*)notification); break;
    case NotificationType::PlayerMove: OnPlayerMove((PlayerMoveNotification*)notification); break;
    }
}