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

    i32 weaponCount = 0;
    bool usingFirstWeapon = true;

    void ProcessMouseMovement();
    void ProcessKeyboardMovement();
    void ChangeWeapon(ActorManager *actorManager);
public:
    Actor *weapons[2];

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnRender(ActorManager* actorManager);
};