#pragma once

#include <containers.h>

struct ComponentBase;
class ActorManager;

struct ComponentListBase
{
    virtual ~ComponentListBase() {}

    virtual ComponentBase* AddComponent() = 0;
    virtual void RemoveComponent(ActorManager* actorManager, ComponentBase* component) = 0;
    virtual void CopyComponent(ComponentBase* dstBase, ComponentBase* srcBase) = 0;
};

template <typename ComponentType>
struct ComponentList : ComponentListBase
{
    BlockAllocator<sizeof(ComponentType)> allocator;

    ComponentBase* AddComponent() override;
    void RemoveComponent(ActorManager* actorManager, ComponentBase* component) override;
    void CopyComponent(ComponentBase* dstBase, ComponentBase* srcBase) override;
};

template <typename ComponentType>
ComponentBase* ComponentList<ComponentType>::AddComponent()
{
    void* buffer = allocator.Alloc();
    ComponentType* component = new (buffer) ComponentType();
    return component;
}

template <typename ComponentType>
void ComponentList<ComponentType>::RemoveComponent(ActorManager* actorManager, ComponentBase* componentBase)
{
    ComponentType* component = (ComponentType*)componentBase;
    component->OnTerminate(actorManager);
    allocator.Free(component);
}

template <typename ComponentType>
void ComponentList<ComponentType>::CopyComponent(ComponentBase* dstBase, ComponentBase* srcBase)
{
    ComponentType* dst = (ComponentType*)dstBase;
    ComponentType* src = (ComponentType*)srcBase;
    *dst = *src;
}