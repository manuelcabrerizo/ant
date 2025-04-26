#pragma once

struct PlayerControllerComponent : public Component<PlayerControllerComponent>
{
    f32 yaw = 0;
    f32 pitch = 0;
    SlotmapKey<Actor> weapons[2];
    i32 weaponCount = 0;
    bool usingFirstWeapon = true;
};