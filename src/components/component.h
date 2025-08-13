#pragma once

#include "../containers.h"
#include "../actor_manager.h"

struct ComponentBase
{
    static int counter;

    SlotmapKey<Actor> owner;
    bool enable = true;
    bool initialized = false;

    ComponentBase* next = nullptr;
    ComponentBase* prev = nullptr;

    virtual ~ComponentBase() {}
    virtual void OnInit(ActorManager *actorManager) {}
    virtual void OnTerminate(ActorManager *actorManager) {}
};

template <typename>
struct Component : public ComponentBase
{
    static int GetID()
    {
        static int id = ++counter;
        return id;
    }
};