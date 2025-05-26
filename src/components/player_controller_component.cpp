#include "player_controller_component.h"
#include "transform_component.h"
#include "camera_component.h"
#include "physics_component.h"
#include "weapon_component.h"
#include "anchor_component.h"
#include "render_component.h"

#include <math/algebra.h>
#include <math/vector3.h>
#include <math/matrix4.h>

#include <input_manager.h>
#include <notification_manager.h>
#include <graphics_manager.h>

#include <platform.h>

void PlayerControllerComponent::OnInit(ActorManager *actorManager)
{
    speed = 20.0f;
    showMouse = false;
    PlatformShowMouse(showMouse);
    
    transform = actorManager->GetComponent<TransformComponent>(owner);
    camera = actorManager->GetComponent<CameraComponent>(owner);
    physics = actorManager->GetComponent<PhysicsComponent>(owner);
    weapon = actorManager->GetComponent<WeaponComponent>(owner);
}

void PlayerControllerComponent::OnTerminate(ActorManager *actorManager)
{
    showMouse = true;
    PlatformShowMouse(showMouse);
}

void PlayerControllerComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    ChangeWeapon(actorManager);
    ProcessMouseMovement();
    ProcessKeyboardMovement();
}

void PlayerControllerComponent::OnLateUpdate(ActorManager *actorManager, f32 dt)
{
     void *data[2];
     data[0] = transform;
     data[1] = physics;
     NotificationManager::Get()->SendNotification(NOTIFICATION_PLAYER_MOVE, (void *)data, sizeof(void *)*2, (void *)this);

    if(InputManager::Get()->MouseButtonJustDown(MOUSE_BUTTON_LEFT))
    {
         const char *message = "te dispare gato\n";
         NotificationManager::Get()->SendNotification(NOTIFICATION_SHOOT, (void *)message, strlen(message), (void *)this);
    }  

    if(InputManager::Get()->MouseButtonDown(MOUSE_BUTTON_LEFT))
    {
         int currentWeapon = usingFirstWeapon ? 0 : 1;
         AnchorComponent *weaponAnchor = actorManager->GetComponent<AnchorComponent>(weapons[currentWeapon]);
         Vector3 targetPosition = camera->GetPosition() + camera->GetFront() * 100.0f;
         GraphicsManager::Get()->DebugDrawLine(weaponAnchor->position, targetPosition);
    }
}

void PlayerControllerComponent::ProcessMouseMovement()
{
    if(InputManager::Get()->KeyJustDown(KEY_ESCAPE))
    {
         showMouse = !showMouse;
         PlatformShowMouse(showMouse);
    }
    if(showMouse == false)
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

         Vector3 dir = Vector3(0.0f, 0.0f, 1.0f);
         transform->direction = Matrix4::TransformVector(Matrix4::RotateX(pitch), dir);
         transform->direction = Matrix4::TransformVector(Matrix4::RotateY(yaw), transform->direction);
         transform->direction.Normalize();
         i32 windowX, windowY, windowW, windowH;
         PlaformGetWindowPos(&windowX, &windowY);
         PlatformClientDimensions(&windowW, &windowH);
         PlaformSetCursorPos(windowX + windowW/2, windowY + windowH/2); 
         InputManager::Get()->SetMousePosition(windowW/2, windowH/2);
         InputManager::Get()->SetMouseLastPosition(windowW/2, windowH/2);
    }
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

void PlayerControllerComponent::ChangeWeapon(ActorManager *actorManager)
{
    RenderComponent *weaponRenderComponents[2];
    weaponRenderComponents[0] = actorManager->GetComponent<RenderComponent>(weapons[0]);
    weaponRenderComponents[1] = actorManager->GetComponent<RenderComponent>(weapons[1]);

    if(InputManager::Get()->KeyJustDown(KEY_1) && !usingFirstWeapon)
    {
         weaponRenderComponents[1]->enable = false;
         weaponRenderComponents[0]->enable = true;    
         weapon->weapon = weapons[0];      
         usingFirstWeapon = true;
    }
    if(InputManager::Get()->KeyJustDown(KEY_2) && usingFirstWeapon)
    {
          weaponRenderComponents[0]->enable = false;   
          weaponRenderComponents[1]->enable = true;
          weapon->weapon = weapons[1];
         usingFirstWeapon = false;
    }
}