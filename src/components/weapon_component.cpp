#include "weapon_component.h"
#include "anchor_component.h"
#include "transform_component.h"
#include "collider_component.h"
#include "bullet_component.h"
#include "render_component.h"

#include <collision.h>

static unsigned long long shootCounter = 0;

void WeaponComponent::OnInit(ActorManager *actorManager)
{
    NotificationManager::Get()->AddListener(this, NotificationType::Shoot);

    am = actorManager;
    transform = owner->GetComponent<TransformComponent>();
    collider = owner->GetComponent<ColliderComponent>();

    bulletPrefab = am->CreateActorFromFile("data/xml/bullet.xml");
    BulletComponent* bullet = bulletPrefab->GetComponent<BulletComponent>();
    bullet->enable = false;
    RenderComponent* render = bulletPrefab->GetComponent<RenderComponent>();
    render->enable = false;
}

void WeaponComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(this, NotificationType::Shoot);
}

void WeaponComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    TransformComponent *weaponTransform = weapon->GetComponent<TransformComponent>();
    AnchorComponent *weaponAnchor = weapon->GetComponent<AnchorComponent>();

    Vector3 front = transform->direction.Normalized();
    Vector3 right = Vector3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
    Vector3 up = front.Cross(right);
    weaponTransform->position = transform->position + front * 0.005f + right * 0.0f + up * -0.3f;
    weaponTransform->direction = transform->direction;
    
    weaponAnchor->position = weaponTransform->position +
         right * weaponAnchor->offset.x +
         up * weaponAnchor->offset.y +
         front * weaponAnchor->offset.z;
}

void WeaponComponent::OnShoot(ShootNotification* notification)
{
    Actor *bullet = am->CloneActor(bulletPrefab);
    TransformComponent *bulletTransform = bullet->GetComponent<TransformComponent>();
    bulletTransform->position = notification->shootPosition;
    bulletTransform->direction = notification->shootDirection;
    am->InitializeNewComponents();

    // Scratch Buffer for collisionData results
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, SCRATCH_MEMORY);

    // Raycast againts the CollisionWorld and hit the enemies
    Ray shootRay;
    shootRay.Init(notification->shootPosition, notification->shootDirection);
    if (CollisionWorld::Get()->Intersect(shootRay, collisionData, collider->GetId()))
    {
        CollisionData& data = collisionData[0];
        if (data.collider->owner->GetTag() == ActorTag::Enemy)
        {
            EnemyHitNotification notification;
            notification.enemy = data.collider->owner;
            NotificationManager::Get()->SendNotification(NotificationType::EnemyHit, &notification);
        }
    }

    // Release the Scratch Buffer
    MemoryManager::Get()->ReleaseFrame(frame);
}

void WeaponComponent::OnNotify(NotificationType type, Notification* notification)
{
    switch (type)
    {
    case NotificationType::Shoot: OnShoot((ShootNotification*)notification); break;
    }
}
