#include "player_controller_component.h"
#include "transform_component.h"
#include "camera_component.h"
#include "physics_component.h"
#include "weapon_component.h"
#include "anchor_component.h"
#include "render_component.h"
#include "collider_component.h"
#include "button_component.h"
#include "portal_component.h"

#include <math/algebra.h>
#include <math/vector3.h>
#include <math/matrix4.h>

#include <input_manager.h>
#include <notification_manager.h>
#include <graphics_manager.h>

#include <platform.h>
#include <collisions/collision_utils.h>
#include <collision.h>

void PlayerControllerComponent::OnInit(ActorManager *actorManager)
{
    speed = 40.0f;    
    transform = owner->GetComponent<TransformComponent>();
    camera = owner->GetComponent<CameraComponent>();
    physics = owner->GetComponent<PhysicsComponent>();
    weapon = owner->GetComponent<WeaponComponent>();
    collider = owner->GetComponent<ColliderComponent>();
    lastPosition = transform->position;

    keyCount = 0;
}   

void PlayerControllerComponent::OnTerminate(ActorManager *actorManager)
{

}

void PlayerControllerComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    ProcessMouseMovement();
    ProcessKeyboardMovement();
    ProcessTriggers();

    PlayerMoveNotification playerNoti;
    playerNoti.position = transform->position;
    playerNoti.velocity = physics->velocity;
    playerNoti.transform = transform;
    NotificationManager::Get()->SendNotification(NotificationType::PlayerMove, &playerNoti);    

    lastPosition = transform->position;

    if (InputManager::Get()->MouseButtonJustDown(MOUSE_BUTTON_LEFT))
    {
        ShootNotification notification;
        notification.shootPosition = camera->GetPosition();
        notification.shootDirection = camera->GetFront();
        NotificationManager::Get()->SendNotification(NotificationType::Shoot, &notification);
    }
}

void PlayerControllerComponent::OnKeyTrigger(Actor* key)
{
    keyCount++;
    key->SetEnable(false);
}

void PlayerControllerComponent::OnButtonTrigger(Actor* button)
{
    ButtonComponent* buttonComponent = button->GetComponent<ButtonComponent>();
    buttonComponent->SetSignal(true);
    SignalNotification notification;
    notification.signable = static_cast<ISignable*>(buttonComponent);
    NotificationManager::Get()->SendNotification(NotificationType::Signal, &notification);
}

void PlayerControllerComponent::OnPortalTrigger(Actor* portal)
{
    PortalComponent* portalComponent = portal->GetComponent<PortalComponent>();
    transform->position = portalComponent->GetDestPosition();
    SetRotation(portalComponent->GetDestRotation());
    physics->velocity = Vector3::zero;
    physics->acceleration = Vector3::zero;
}

void PlayerControllerComponent::OnEnemyCollision(Actor* enemy)
{

}

void PlayerControllerComponent::OnEndTrigger(Actor* endTrigger)
{
    PlayerWinNotification notification;
    NotificationManager::Get()->SendNotification(NotificationType::PlayerWin, &notification);
}

void PlayerControllerComponent::ProcessMouseMovement()
{
    yaw += InputManager::Get()->MouseXMovement() * 0.001f;
    pitch += InputManager::Get()->MouseYMovement() * 0.001f;
    f32 limit = Radians(89.0f);
    if(pitch > limit)
    {
        pitch = limit;
    }
    if(pitch < -limit)
    {
        pitch = -limit;
    }

    transform->rotation.x = pitch;
    transform->rotation.y = yaw;

    i32 windowX, windowY, windowW, windowH;
    PlaformGetWindowPos(&windowX, &windowY);
    PlatformClientDimensions(&windowW, &windowH);
    PlaformSetCursorPos(windowX + windowW/2, windowY + windowH/2); 
    InputManager::Get()->SetMousePosition(windowW/2, windowH/2);
    InputManager::Get()->SetMouseLastPosition(windowW/2, windowH/2);
}

void PlayerControllerComponent::ProcessKeyboardMovement()
{
    Vector3 moveDirection = Vector3(0.0f);
    if(InputManager::Get()->KeyDown(KEY_A))
    {
         moveDirection -= camera->GetRight();
    }
    if(InputManager::Get()->KeyDown(KEY_D))
    {
         moveDirection += camera->GetRight();
    }
    if(InputManager::Get()->KeyDown(KEY_W))
    {
         moveDirection += camera->GetWorldFront();
    }
    if(InputManager::Get()->KeyDown(KEY_S))
    {
         moveDirection -= camera->GetWorldFront();
    }
    
    if(moveDirection.MagnitudeSq() > 0.0f)
    {
         moveDirection.Normalize();
    }

    // Jump code
    if(physics->grounded && InputManager::Get()->KeyJustDown(KEY_SPACE))
    {
         physics->velocity.y = 0.0f;
         physics->velocity += Vector3(0.0f, 9.8f*1.5f, 0.0f);                    
    }

    Vector3 movement = (moveDirection * speed);
    physics->acceleration = movement;
}

void PlayerControllerComponent::ProcessTriggers()
{
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);

    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, SCRATCH_MEMORY);

    collisionData.Clear();
    if (CollisionWorld::Get()->Intersect(collider, collisionData))
    {
        for (int i = 0; i < collisionData.size; i++)
        {
            if (collisionData[i].collider->IsTrigger())
            {
                Actor* other = collisionData[i].collider->owner;
                switch (other->GetTag())
                {
                case ActorTag::Key: OnKeyTrigger(other); break;
                case ActorTag::Enemy: OnEnemyCollision(other); break;
                case ActorTag::Button: OnButtonTrigger(other); break;
                case ActorTag::Portal: OnPortalTrigger(other); break;
                case ActorTag::EndTrigger: OnEndTrigger(other); break;
                }
            }
        }

    }

    MemoryManager::Get()->ReleaseFrame(frame);
}