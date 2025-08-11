#include "enemy_component.h"
#include "transform_component.h"
#include "physics_component.h"

#include <math.h>
#include <math/algebra.h>

void EnemyComponent::OnInit(ActorManager *actorManager)
{
    transform = actorManager->GetComponent<TransformComponent>(owner);
    physics = actorManager->GetComponent<PhysicsComponent>(owner);

    //movement.SetCharacter(&character);
    //movement.SetTarget(&target);
    //movement.SetMaxAcceleration(20.0f);
    //movement.SetMaxPrediction(2.0f);
    //movement.SetMaxSpeed(10.0f);
    //movement.SetTargetRadius(2.0f);
    //movement.SetSlowRadius(4.0f);
    //movement.SetTimeToTarget(0.5f);

    //turn.SetCharacter(&character);
    //turn.SetTarget(&target);
    //turn.SetMaxAngularAcceleration(200.0f);
    //turn.SetMaxRotation(50.0f);
    //turn.SetTargetRadius(pi<float>() * 0.1f);
    //turn.SetSlowRadius(pi<float>() * 0.5f);
    //turn.SetTimeToTarget(0.001f);

    wander.SetCharacter(&character);
    wander.SetTarget(&target);
    wander.SetWanderOffset(1.0f);
    wander.SetWanderRadius(5.0f);
    wander.SetWanderRate(0.5f);
    wander.SetWanderOrientation(0.0f);
    wander.SetMaxAcceleration(5.0f);
    wander.SetMaxAngularAcceleration(5.0f);
    wander.SetMaxRotation(10.0f);
    wander.SetTargetRadius(ANT_PI * 0.1f);
    wander.SetSlowRadius(ANT_PI * 0.5f);
    wander.SetTimeToTarget(0.001f);

    NotificationManager::Get()->AddListener(this, NOTIFICATION_PLAYER_MOVE);
}

void EnemyComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(this, NOTIFICATION_PLAYER_MOVE);
}

void EnemyComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    if(physics->grounded)
    {
        character.position = transform->position;
        character.velocity = physics->velocity;
        character.velocity.y = 0.0f;
        SteeringOutput steering = wander.GetSteering();

        physics->acceleration = steering.linear;
        character.rotation += steering.angular * dt;
        character.rotation *= powf(0.001f, dt); // angular drag
        character.orientation += character.rotation * dt;
        transform->direction = Vector3(-sinf(character.orientation), 0.0f, cosf(character.orientation)).Normalized();
    }
}

void EnemyComponent::OnNotify(NotificationType type, void *data, size_t size, void *sender)
{
    void **components = (void **)data;
    TransformComponent *playerTransform = (TransformComponent *)components[0];
    PhysicsComponent *playerPhysics = (PhysicsComponent *)components[1];
    target.position = playerTransform->position;
    target.velocity = playerPhysics->velocity;
    target.orientation = Kinematic::GetNewOrientation(target.orientation, playerTransform->direction);
}