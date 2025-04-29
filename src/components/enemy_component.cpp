void EnemyComponent::OnInit(ActorManager *actorManager)
{
    transform = actorManager->GetComponent<TransformComponent>(owner);
    physics = actorManager->GetComponent<PhysicsComponent>(owner);

    movement.SetCharacter(&character);
    //movement.SetTarget(&target);
    movement.SetMaxSpeed(1.0f);
    movement.SetMaxRotation(pi<f32>() * 4.0f);
    //movement.SetRadius(1.0f);
    //movement.SetTimeToTarget(0.25f);

    playerMoveNotification = NotificationManager::Get()->AddListener(this, NOTIFICATION_PLAYER_MOVE);
}

void EnemyComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(playerMoveNotification, NOTIFICATION_PLAYER_MOVE);
}

void EnemyComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    if(physics->grounded)
    {
        character.position = transform->position;
        KinematicSteeringOutput steering = movement.GetSteering();
        physics->velocity = steering.velocity;
        character.orientation += steering.rotation * dt;
        transform->direction = normalize(vec3(-sinf(character.orientation), 0.0f, cosf(character.orientation)));
    }
}

void EnemyComponent::OnNotify(NotificationType type, void *data, size_t size, void *sender)
{
    vec3 *position = (vec3 *)data;
    target.position = *position;  
}