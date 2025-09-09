#pragma once

#include "component.h"

class TransformComponent;
class CameraComponent;
class PhysicsComponent;
class WeaponComponent;

class PlayerControllerComponent : public Component<PlayerControllerComponent>
{
private:
    TransformComponent* transform;
    CameraComponent* camera;
    PhysicsComponent* physics;
    WeaponComponent* weapon;

    f32 yaw = 0;
    f32 pitch = 0;
    float speed;
    Vector3 lastPosition;

    void ProcessMouseMovement();
    void ProcessKeyboardMovement();
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