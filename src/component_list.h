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
    BlockAllocator<sizeof(ComponentType)> allocator;
    void RemoveComponent(ActorManager* actorManager, ComponentBase* component) override;
};

template <typename ComponentType>
void ComponentList<ComponentType>::RemoveComponent(ActorManager* actorManager, ComponentBase* componentBase)
{
    ComponentType* component = (ComponentType*)componentBase;
    component->OnTerminate(actorManager);
    allocator.Free(component);
}