#include "weapon_component.h"
#include "anchor_component.h"
#include "transform_component.h"
#include "collider_component.h"
#include "bullet_component.h"
#include "render_component.h"
#include "camera_component.h"

#include <collision.h>

#include <asset_managers/texture_manager.h>


static unsigned long long shootCounter = 0;

void WeaponComponent::OnInit(ActorManager *actorManager)
{
    NotificationManager::Get()->AddListener(this, NotificationType::Shoot);

    am = actorManager;
    transform = owner->GetComponent<TransformComponent>();
    collider = owner->GetComponent<ColliderComponent>();
    camera = owner->GetComponent<CameraComponent>();

    bulletPrefab = am->CreateActorFromFile("data/xml/bullet.xml");
    BulletComponent* bullet = bulletPrefab->GetComponent<BulletComponent>();
    bullet->enable = false;
    RenderComponent* render = bulletPrefab->GetComponent<RenderComponent>();
    render->enable = false;

    TextureManager::Get()->Load("Blood", "data/textures/blood2.png");
    particleSystem.Init(500, "Blood", FRAME_MEMORY);
    particleSystem.SetPosition(transform->position);
    particleSystem.Stop();
}

void WeaponComponent::OnTerminate(ActorManager *actorManager)
{
    particleSystem.Terminate();
    TextureManager::Get()->Unload("Blood");

    NotificationManager::Get()->RemoveListener(this, NotificationType::Shoot);
}

void WeaponComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    particleSystem.Update(camera->GetPosition(), dt);

    if (timeToSpawn < 0.0f)
    {
        particleSystem.Stop();
    }
    timeToSpawn -= dt;

    TransformComponent *weaponTransform = weapon->GetComponent<TransformComponent>();
    AnchorComponent *weaponAnchor = weapon->GetComponent<AnchorComponent>();

    Vector3 front = transform->direction.Normalized();
    Vector3 right = Vector3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
    Vector3 up = front.Cross(right);
    weaponTransform->position = transform->position + front * 0.25f + right * 0.1f + up * -0.2f;
    weaponTransform->direction = transform->direction;
    
    weaponAnchor->position = weaponTransform->position +
         right * weaponAnchor->offset.x +
         up * weaponAnchor->offset.y +
         front * weaponAnchor->offset.z;
}

void WeaponComponent::OnRender(ActorManager* actorManager)
{
    GraphicsManager::Get()->SetDepthStencilOnWriteMaskZero();
    GraphicsManager::Get()->SetAlphaBlending();
    particleSystem.Render();
    GraphicsManager::Get()->SetBlendingOff();
    GraphicsManager::Get()->SetDepthStencilOn();
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
            particleSystem.SetPosition(shootRay.GetOrigin() + shootRay.GetDirection() * data.t);
            particleSystem.Play();
            timeToSpawn = 0.15f;

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
