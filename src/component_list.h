#pragma once

#include <containers.h>

struct ComponentBase;
class ActorManager;

struct ComponentListBase
{
    virtual void RemoveComponent(ActorManager* actorManager, ComponentBase* component) = 0;
    virtual ~ComponentListBase() {}
};

template <typename ComponentType>
struct ComponentList : ComponentListBase
{
    ObjectAllocator<ComponentType> allocator;
    ComponentBase* first;

    void RemoveComponent(ActorManager* actorManager, ComponentBase* component) override;
};

template <typename ComponentType>
void ComponentList<ComponentType>::RemoveComponent(ActorManager* actorManager, ComponentBase* componentBase)
{
    ComponentType* component = (ComponentType*)componentBase;

    component->OnTerminate(actorManager);

    if (component->prev == nullptr)
    {
        first = component->next;
        if (first)
        {
            first->prev = nullptr;
        }
    }
    else if (component->next == nullptr)
    {
        component->prev->next = nullptr;
    }
    else
    {
        component->prev->next = component->next;
        component->next->prev = component->prev;
    }

    component->prev = nullptr;
    component->next = nullptr;

    allocator.Free(component);
}