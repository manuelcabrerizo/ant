void EnemyComponent::OnInit(ActorManager *actorManager)
{
    transform = actorManager->GetComponent<TransformComponent>(owner);
    playerPosition = vec3(0.0f);

    playerMoveNotification = NotificationManager::Get()->AddListener(this, NOTIFICATION_PLAYER_MOVE);
}

void EnemyComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(playerMoveNotification, NOTIFICATION_PLAYER_MOVE);
}

void EnemyComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    vec3 toPlayer = transform->position - playerPosition;
    toPlayer.y = 0;
    transform->direction = normalize(toPlayer);
}

void EnemyComponent::OnNotify(NotificationType type, void *data, size_t size, void *sender)
{
    vec3 *position = (vec3 *)data;
    playerPosition = *position;  
}