#pragma once

#include "component.h"

class TransformComponent;
class CameraComponent;
class PhysicsComponent;
class WeaponComponent;

class PlayerControllerComponent : public Component<PlayerControllerComponent>
{
private:
    f32 yaw = 0;
    f32 pitch = 0;
    float speed;
    bool showMouse;

    i32 weaponCount = 0;
    bool usingFirstWeapon = true;

    TransformComponent *transform;
    CameraComponent *camera;
    PhysicsComponent *physics;
    WeaponComponent *weapon;

    void ProcessMouseMovement();
    void ProcessKeyboardMovement();
    void ChangeWeapon(ActorManager *actorManager);
public:
    Actor *weapons[2];

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
};