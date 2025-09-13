#include "weapon_component.h"
#include "anchor_component.h"
#include "transform_component.h"
#include "collider_component.h"
#include "render_component.h"
#include "camera_component.h"

#include <collision.h>

#include <asset_managers/texture_manager.h>


static unsigned long long shootCounter = 0;

void WeaponComponent::OnInit(ActorManager *actorManager)
{
    NotificationManager::Get()->AddListener(this, NotificationType::Shoot);

    transform = owner->GetComponent<TransformComponent>();
    collider = owner->GetComponent<ColliderComponent>();
    camera = owner->GetComponent<CameraComponent>();

    TextureManager::Get()->Load("Blood", "data/textures/blood2.png", FRAME_MEMORY);
    bloodPs.SetPosition(transform->position);
    bloodPs.SetTexture(TextureManager::Get()->Get("Blood"));
    bloodPs.Stop();

    TextureManager::Get()->Load("Shoot", "data/textures/shoot.png", FRAME_MEMORY);
    shootPs.SetPosition(transform->position);
    shootPs.SetTexture(TextureManager::Get()->Get("Shoot"));
    shootPs.Stop();
}

void WeaponComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(this, NotificationType::Shoot);
}

#include <windows.h>

void WeaponComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    // TODO: save this components, dont ask for them every frame
    TransformComponent* weaponTransform = weapon->GetComponent<TransformComponent>();
    AnchorComponent* weaponAnchor = weapon->GetComponent<AnchorComponent>();

    Vector3 front = transform->direction.Normalized();
    Vector3 right = Vector3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
    Vector3 up = front.Cross(right);
    weaponTransform->position = transform->position + front * 0.25f + right * 0.1f + up * -0.2f;
    weaponTransform->direction = transform->direction;
    
    weaponAnchor->position = weaponTransform->position  +
        right * weaponAnchor->offset.x +
        up * weaponAnchor->offset.y +
        front * weaponAnchor->offset.z; 

    shootPs.SetPosition(weaponAnchor->position);

    bloodPs.Update(camera->GetPosition(), dt);
    shootPs.Update(camera->GetPosition(), dt);

    if (bloodTimer < 0.0f)
    {
        bloodPs.Stop();
    }
    bloodTimer -= dt;

    if (shootTimer < 0.0f)
    {
        shootPs.Stop();
    }
    shootTimer -= dt;
}

void WeaponComponent::OnSetRenderState()
{
    GraphicsManager::Get()->SetRasterizerStateCullBack();
    GraphicsManager::Get()->SetDepthStencilOnWriteMaskZero();
}

void WeaponComponent::OnRender(ActorManager* actorManager)
{
    GraphicsManager::Get()->SetAlphaBlending();
    bloodPs.Render();
    GraphicsManager::Get()->SetAdditiveBlending();
    shootPs.Render();
}

void WeaponComponent::OnShoot(ShootNotification* notification)
{
    // Render shoot particles
    shootPs.SetPosition(notification->shootPosition);
    shootPs.Play();
    shootTimer = shootDuration;

    // Scratch Buffer for collisionData results
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, SCRATCH_MEMORY);

    // Raycast againts the CollisionWorld and hit the enemies
    Ray shootRay;
    shootRay.Init(notification->shootPosition, notification->shootDirection);
    if (CollisionWorld::Get()->Intersect(shootRay, collisionData, owner))
    {
        CollisionData& data = collisionData[0];
        if (data.collider->owner->GetTag() == ActorTag::Enemy)
        {
            bloodPs.SetPosition(shootRay.GetOrigin() + shootRay.GetDirection() * data.t);
            bloodPs.Play();
            bloodTimer = bloodDuration;

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
