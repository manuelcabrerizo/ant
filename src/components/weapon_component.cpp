#include "weapon_component.h"
#include "anchor_component.h"
#include "transform_component.h"

#include <stdio.h>

void WeaponComponent::OnInit(ActorManager *actorManager)
{
    transform = actorManager->GetComponent<TransformComponent>(owner);
    NotificationManager::Get()->AddListener(this, NotificationType::Shoot);
}

void WeaponComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(this, NotificationType::Shoot);
}

void WeaponComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    TransformComponent *weaponTransform = actorManager->GetComponent<TransformComponent>(weapon);
    AnchorComponent *weaponAnchor = actorManager->GetComponent<AnchorComponent>(weapon);

    Vector3 front = transform->direction.Normalized();
    Vector3 right = Vector3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
    Vector3 up = front.Cross(right);
    weaponTransform->position = transform->position + front * 0.1f + right * 0.0f + up * -0.3f;
    weaponTransform->direction = transform->direction;
    
    weaponAnchor->position = weaponTransform->position +
         right * weaponAnchor->offset.x +
         up * weaponAnchor->offset.y +
         front * weaponAnchor->offset.z;
}

void WeaponComponent::OnNotify(NotificationType type, Notification *notification) 
{
    switch(type)
    {
        case NotificationType::Shoot:
        {
            ShootNotification* shoot = (ShootNotification*)notification;
        } break;
    }
}