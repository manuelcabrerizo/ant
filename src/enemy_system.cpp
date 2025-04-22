void EnemySystem::Init()
{
    playerPosition = vec3(0.0f);
}

void EnemySystem::Update(ActorManager *actorManager, f32 dt)
{
    Array<EnemyComponent>& enemies = actorManager->GetComponents<EnemyComponent>();
    for(i32 i = 0; i < enemies.size; ++i)
    {
        EnemyComponent *enemy = &enemies[i];
        if(!enemy->enable)
        {
            continue;
        }

        SlotmapKey<Actor> actor = enemies[i].owner;
        TransformComponent *transform = actorManager->GetComponent<TransformComponent>(actor);

        vec3 toPlayer = transform->position - playerPosition;
        toPlayer.y = 0;
        transform->direction = normalize(toPlayer);
    }
}

void EnemySystem::Terminate()
{

}

void EnemySystem::OnNotify(NotificationType type, void *data, size_t size, void *sender)
{
       vec3 *position = (vec3 *)data;
       playerPosition = *position;  
}
