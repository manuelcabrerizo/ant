#pragma once

#include "../containers.h"
#include "../actor.h"

struct ComponentBase
{
    static i32 counter;

    SlotmapKey<Actor> owner;
    bool enable = true;
    bool initialized = false;

    virtual ~ComponentBase() {}
    virtual void OnInit(ActorManager *actorManager) {}
    virtual void OnTerminate(ActorManager *actorManager) {}
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