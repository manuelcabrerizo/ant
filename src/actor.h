#pragma once

class ActorManager;
struct ComponentBase;

#include "containers.h"

struct Actor
{
     HashMap<SlotmapKeyBase> componentsMap;
     Array<i32> componentsIds;
};

struct ComponentStorageBase
{
     virtual void RemoveComponent(ActorManager *actorManager, SlotmapKeyBase keyBase) = 0;
     virtual ~ComponentStorageBase() {}
};

template <typename ComponentType>
struct ComponentStorage : ComponentStorageBase
{
     Slotmap<ComponentType> components;
     void RemoveComponent(ActorManager *actorManager, SlotmapKeyBase keyBase) override;
};

class ActorManager
{
private:
     i32 memoryType = STATIC_MEMORY;
     i32 componentCount = 0;
     i32 maxActorCount = 0;
     i32 maxComponentCount = 0;

     Slotmap<Actor> actors;
     HashMap<ComponentStorageBase *> componentStorageMap;
     Array<ComponentBase *> componentsToInit;
public:
     void BeingInitialization(i32 actorCount, i32 componentCount, i32 memoryType);
     void EndInitialization();

     void Terminate();
     void Clear();

     template <typename ComponentType, i32 Count>
     void AddComponentType();


     template <typename ComponentType>
     Array<ComponentType>& GetComponents();

     SlotmapKey<Actor> CreateActor(i32 componentCount);
     SlotmapKey<Actor> CreateActorFromFile(const char *filepath);
     void DestroyActor(SlotmapKey<Actor> actorKey);
     Actor *GetActor(SlotmapKey<Actor> actorKey);
    
     template <typename ComponentType>
     void AddComponent(SlotmapKey<Actor> actorKey, ComponentType component);

     template <typename ComponentType>
     void RemoveComponent(SlotmapKey<Actor> actorKey);
     void RemoveComponentById(SlotmapKey<Actor> actorKey, i32 id);
     void RemoveComponentById(Actor *actor, i32 id);
     
     template <typename ComponentType>
     ComponentType *GetComponent(SlotmapKey<Actor> actorKey);

     void InitializeNewComponents();

     template<typename ComponentType>
     void UpdateComponents(f32 dt);

     template<typename ComponentType>
     void LateUpdateComponents(f32 dt);

     template<typename ComponentType>
     void RenderComponents();
};

// template functions
template <typename ComponentType>
void ComponentStorage<ComponentType>::RemoveComponent(ActorManager* actorManager, SlotmapKeyBase keyBase)
{
    SlotmapKey<ComponentType> key = FromKeyBase<ComponentType>(keyBase);
    ComponentType* component = components.Get(key);
    component->OnTerminate(actorManager);
    components.Remove(key);
}

template <typename ComponentType, i32 Count>
void ActorManager::AddComponentType()
{
    ASSERT(componentStorageMap.Count() + 1 <= componentCount);

    void* buffer = MemoryManager::Get()->Alloc(sizeof(ComponentStorage<ComponentType>), memoryType);
    ComponentStorage<ComponentType>* componentStorage = new (buffer) ComponentStorage<ComponentType>();
    componentStorage->components.Init(Count, memoryType);
    componentStorageMap.Add(ComponentType::GetID(), (ComponentStorageBase*)componentStorage);
    maxComponentCount += Count;
}

template <typename ComponentType>
Array<ComponentType>& ActorManager::GetComponents()
{
    ComponentStorage<ComponentType>* compoentStorage =
        (ComponentStorage<ComponentType> *)(*componentStorageMap.Get(ComponentType::GetID()));
    return *compoentStorage->components.GetArray();
}

template <typename ComponentType>
void ActorManager::AddComponent(SlotmapKey<Actor> actorKey, ComponentType component)
{
    component.owner = actorKey;

    ComponentStorage<ComponentType>* compoentStorage =
        (ComponentStorage<ComponentType> *)(*componentStorageMap.Get(ComponentType::GetID()));

    SlotmapKey<ComponentType> key = compoentStorage->components.Add(component);

    Actor* actor = GetActor(actorKey);
    SlotmapKeyBase keyBase = FromKey<ComponentType>(key);
    actor->componentsMap.Add(ComponentType::GetID(), keyBase);
    actor->componentsIds.Push(ComponentType::GetID());

    ComponentBase* componentBase = (ComponentBase*)GetComponent<ComponentType>(actorKey);
    componentsToInit.Push(componentBase);
}

template <typename ComponentType>
void ActorManager::RemoveComponent(SlotmapKey<Actor> actorKey)
{
    ComponentType* component = GetComponent<ComponentType>(actorKey);
    component->OnTerminate(this);

    ComponentStorage<ComponentType>* compoentStorage =
        (ComponentStorage<ComponentType> *)(*componentStorageMap.Get(ComponentType::GetID()));

    Actor* actor = GetActor(actorKey);
    SlotmapKeyBase keyBase = *actor->componentsMap.Get(ComponentType::GetID());
    SlotmapKey<ComponentType> key = FromKeyBase<ComponentType>(keyBase);
    ASSERT(key.gen != INVALID_KEY);
    compoentStorage->components.Remove(key);
    actor->componentsMap.Remove(ComponentType::GetID());
    i32 foundIndex = -1;
    for (i32 i = 0; i < actor->componentsIds.size; ++i)
    {
        if (actor->componentsIds[i] == ComponentType::GetID())
        {
            foundIndex = i;
        }
    }
    if (foundIndex >= 0)
    {
        actor->componentsIds[foundIndex] = actor->componentsIds[actor->componentsIds.size - 1];
        actor->componentsIds[actor->componentsIds.size - 1] = 0;
        actor->componentsIds.size--;
    }
}

template <typename ComponentType>
ComponentType* ActorManager::GetComponent(SlotmapKey<Actor> actorKey)
{
    ComponentStorage<ComponentType>* compoentStorage =
        (ComponentStorage<ComponentType> *)(*componentStorageMap.Get(ComponentType::GetID()));

    Actor* actor = GetActor(actorKey);
    SlotmapKeyBase keyBase = *actor->componentsMap.Get(ComponentType::GetID());
    SlotmapKey<ComponentType> key = FromKeyBase<ComponentType>(keyBase);
    ASSERT(key.gen != INVALID_KEY);
    return compoentStorage->components.Get(key);
}

template<typename ComponentType>
void ActorManager::UpdateComponents(f32 dt)
{
    Array<ComponentType>& components = GetComponents<ComponentType>();
    for (u32 i = 0; i < components.size; ++i)
    {
        if (components[i].initialized && components[i].enable)
        {
            components[i].OnUpdate(this, dt);
        }
    }
}

template<typename ComponentType>
void ActorManager::LateUpdateComponents(f32 dt)
{
    Array<ComponentType>& components = GetComponents<ComponentType>();
    for (u32 i = 0; i < components.size; ++i)
    {
        if (components[i].initialized && components[i].enable)
        {
            components[i].OnLateUpdate(this, dt);
        }
    }
}

template<typename ComponentType>
void ActorManager::RenderComponents()
{
    Array<ComponentType>& components = GetComponents<ComponentType>();
    for (u32 i = 0; i < components.size; ++i)
    {
        if (components[i].initialized && components[i].enable)
        {
            components[i].OnRender(this);
        }
    }
}