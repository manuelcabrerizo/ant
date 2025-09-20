#pragma once

#include "component.h"

class TransformComponent;
class CameraComponent;
class PhysicsComponent;
class WeaponComponent;
class ColliderComponent;

class PlayerControllerComponent : public Component<PlayerControllerComponent>
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

    void OnKeyTrigger(Actor* key);
    void OnButtonTrigger(Actor* button);
    void OnPortalTrigger(Actor* portal);
    void OnEnemyCollision(Actor* enemy);

    void ProcessMouseMovement();
    void ProcessKeyboardMovement();
    void ProcessTriggers();
public:
    Actor *weaponActor;

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnRender(ActorManager* actorManager);

    void SetRotation(float angle)
    {
        yaw = angle;
        pitch = 0;
    }
};