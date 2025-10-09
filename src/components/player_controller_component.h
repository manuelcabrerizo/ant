#pragma once

#include "component.h"
#include <notification_manager.h>


class TransformComponent;
class CameraComponent;
class PhysicsComponent;
class WeaponComponent;
class ColliderComponent;

class PlayerControllerComponent : public Component<PlayerControllerComponent>, INotificable
{
private:
    TransformComponent* transform = nullptr;
    CameraComponent* camera = nullptr;
    PhysicsComponent* physics = nullptr;
    WeaponComponent* weapon = nullptr;
    ColliderComponent* collider = nullptr;

    f32 yaw = 0;
    f32 pitch = 0;
    float speed;
    Vector3 lastPosition;
    int life = 0;
    int maxLife = 100;

    bool isImmortal = false;
    float immortalTime = 0.5f;
    float immortalTimer = 0.0f;

    void OnButtonTrigger(Actor* button);
    void OnPortalTrigger(Actor* portal);
    void OnAmmoTrigger(Actor* ammo);
    void OnEndTrigger(Actor* endTrigger);
    void OnDeadTrigger(Actor* deadTrigger);

    void ProcessMouseMovement();
    void ProcessKeyboardMovement();
    void ProcessTriggers();

    void SetLife(int life);
public:
    Actor *weaponActor;

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnRender(ActorManager* actorManager);
    void OnNotify(NotificationType type, Notification* notification) override;

    void SetRotation(float angle)
    {
        yaw = angle;
        pitch = 0;
    }
};