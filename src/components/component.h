#pragma once

struct ComponentBase
{
    static i32 counter;
    bool enable = true;
    SlotmapKey<Actor> owner;
};

template <typename>
struct Component : public ComponentBase
{
    static i32 GetID()
    {
        static i32 id = ++counter;
        return id;
    }
};

#include "components.h"