#pragma once

struct WeaponComponent : public Component<WeaponComponent>
{
    SlotmapKey<Actor> weapon;
};
