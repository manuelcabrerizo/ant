#pragma once

#include <containers.h>

struct ComponentBase;
class ActorManager;

struct ComponentListBase
{
    virtual ~ComponentListBase() {}
    virtual void CopyComponent(ComponentBase* dstBase, ComponentBase* srcBase) = 0;
    virtual void Clear(ActorManager* actorManager) = 0;
};


template <typename ComponentType>
class ComponentList : ComponentListBase
{
private:
    Array<ComponentType> components;
public:
    void Init(unsigned int size, int memoryType);
    ComponentType* AddComponent(Actor* actor, const ComponentType& value);
    void Clear(ActorManager* actorManager) override;
    void CopyComponent(ComponentBase* dstBase, ComponentBase* srcBase) override;

    void UpdateComponents(ActorManager* actorManager, float dt);
    void LateUpdateComponents(ActorManager* actorManager, float dt);
    void RenderComponents(ActorManager* actorManager);
};

template <typename ComponentType>
void ComponentList<ComponentType>::Init(unsigned int size, int memoryType)
{
    components.Init(size, memoryType);
}

template <typename ComponentType>
ComponentType* ComponentList<ComponentType>::AddComponent(Actor* actor, const ComponentType& value)
{
    components.Push(value);
    ComponentType* component = &components[components.size - 1];
    component->owner = actor;
    return component;
}


template <typename ComponentType>
void ComponentList<ComponentType>::Clear(ActorManager* actorManager)
{
    for (int i = 0; i < components.size; i++)
    {
        components[i].OnTerminate(actorManager);
    }
    components.Clear();
}

template <typename ComponentType>
void ComponentList<ComponentType>::CopyComponent(ComponentBase* dstBase, ComponentBase* srcBase)
{
    ComponentType* dst = (ComponentType*)dstBase;
    ComponentType* src = (ComponentType*)srcBase;
    *dst = *src;
}

template <typename ComponentType>
void ComponentList<ComponentType>::UpdateComponents(ActorManager* actorManager, float dt)
{
    for (int i = 0; i < components.size; i++)
    {
        ComponentType* component = &components[i];
        if (component->initialized && component->enable)
        {
            component->OnUpdate(actorManager, dt);
        }
    }
}

template <typename ComponentType>
void ComponentList<ComponentType>::LateUpdateComponents(ActorManager* actorManager, float dt)
{
    for (int i = 0; i < components.size; i++)
    {
        ComponentType* component = &components[i];
        if (component->initialized && component->enable)
        {
            component->OnLateUpdate(actorManager, dt);
        }
    }
}

template <typename ComponentType>
void ComponentList<ComponentType>::RenderComponents(ActorManager* actorManager)
{
    for (int i = 0; i < components.size; i++)
    {
        ComponentType* component = &components[i];
        if (component->initialized && component->enable)
        {
            component->OnRender(actorManager);
        }
    }
}