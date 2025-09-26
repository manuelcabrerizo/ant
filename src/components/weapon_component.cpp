#include "weapon_component.h"
#include "anchor_component.h"
#include "transform_component.h"
#include "collider_component.h"
#include "render_component.h"
#include "camera_component.h"
#include "physics_component.h"

#include <collision.h>
#include <asset_managers/texture_manager.h>
#include <math/algebra.h>


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

    animationTime = 0;
    animationOffsetY = (sinf(animationTime) * 0.5f + 0.5f) * 0.015f;
    animationOffsetX = sinf(animationTime * 0.5f) * 0.01f;
    float t = 1.0f - shootAnimationTime;
    animationOffsetZ = fmax((sinf(t * ANT_PI)), 0) * -0.075f;
}

void WeaponComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(this, NotificationType::Shoot);
}

void WeaponComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    // TODO: save this components, dont ask for them every frame
    TransformComponent* weaponTransform = weapon->GetComponent<TransformComponent>();
    AnchorComponent* weaponAnchor = weapon->GetComponent<AnchorComponent>();

    Matrix4 rotMat = Matrix4::TransformFromEuler(transform->rotation);
    Vector3 front = Matrix4::TransformVector(rotMat, Vector3::forward);
    Vector3 right = Matrix4::TransformVector(rotMat, Vector3::right);
    Vector3 up = Matrix4::TransformVector(rotMat, Vector3::up);

    weaponTransform->position = transform->position + front * 0.25f + right * 0.1f + up * -0.2f;
    weaponTransform->rotation = transform->rotation;

    // Weapon movement animation
    PhysicsComponent* physics = owner->GetComponent<PhysicsComponent>();
    if (physics->acceleration.MagnitudeSq() > 0)
    {
        animationOffsetY = (sinf(animationTime) * 0.5f + 0.5f) * 0.015f;
        animationOffsetX = sinf(animationTime * 0.5f) * 0.01f;
        animationTime += (physics->acceleration.Magnitude() * 0.3f) * dt;
    }

    animationOffsetZ = 0.0f;
    if (shootAnimationTime > 0.0f)
    {
        float t = 1.0f - shootAnimationTime;
        animationOffsetZ = fmax((sinf(t * ANT_PI)), 0) * -0.075f;
        shootAnimationTime -= 4.0f * dt;
    }

    weaponTransform->position += right * animationOffsetX;
    weaponTransform->position += up * animationOffsetY;
    weaponTransform->position += front * animationOffsetZ;

    
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
    if (shootAnimationTime > 0.0f)
    {
        return;
    }

    // Render shoot particles
    shootPs.SetPosition(notification->shootPosition);
    shootPs.Play();
    shootTimer = shootDuration;

    // Start shoot Aimation
    shootAnimationTime = 1.0f;

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
