void WeaponComponent::OnInit(ActorManager *actorManager)
{
    transform = actorManager->GetComponent<TransformComponent>(owner);
    shootNotification = NotificationManager::Get()->AddListener(this, NOTIFICATION_SHOOT);
}

void WeaponComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(shootNotification, NOTIFICATION_SHOOT);
}

void WeaponComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    TransformComponent *weaponTransform = actorManager->GetComponent<TransformComponent>(weapon);
    AnchorComponent *weaponAnchor = actorManager->GetComponent<AnchorComponent>(weapon);

    vec3 front = normalize(transform->direction);
    vec3 right = normalize(cross(vec3(0.0f, 1.0f, 0.0f), front));
    vec3 up = cross(front, right);
    weaponTransform->position = transform->position + front * 2.15f + right * 0.05f + up * -0.075f;
    weaponTransform->direction = transform->direction;
    
    weaponAnchor->position = weaponTransform->position +
         right * weaponAnchor->offset.x +
         up * weaponAnchor->offset.y +
         front * weaponAnchor->offset.z;
}

void WeaponComponent::OnNotify(NotificationType type, void *data, size_t size, void *sender) 
{
    switch(type)
    {
        case NOTIFICATION_SHOOT:
        {
            printf((const char *)data);
        } break;
    }
}