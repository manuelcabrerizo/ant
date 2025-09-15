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

#include "component_list.tpp"