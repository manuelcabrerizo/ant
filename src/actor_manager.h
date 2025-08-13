#pragma once

#include <containers.h>
#include <actor.h>
#include <component_list.h>

class ActorManager
{
private:
    i32 memoryType = STATIC_MEMORY;
    i32 componentTypeCount = 0;
    i32 maxActorCount = 0;
    i32 maxComponentCount = 0;

    Slotmap<Actor> actors;
    Array<SlotmapKey<Actor>> toRemove;

    HashMap<ComponentListBase*> componentListMap;
    Array<ComponentBase*> componentsToInit;

public:
    void BeingInitialization(i32 actorCount, i32 componentTypeCount, i32 memoryType);
    void EndInitialization();

    void Terminate();
    void Clear();

    template <typename ComponentType, i32 Count>
    void AddComponentType();
    template <typename ComponentType>
    ComponentList<ComponentType> *GetComponents();

    SlotmapKey<Actor> CreateActor(i32 componentCount);
    SlotmapKey<Actor> CreateActorFromFile(const char* filepath);
    SlotmapKey<Actor> CloneActor(SlotmapKey<Actor> actorKey);
    void DestroyActor(SlotmapKey<Actor> actorKey);
    Actor* GetActor(SlotmapKey<Actor> actorKey);

    template <typename ComponentType>
    void AddComponent(SlotmapKey<Actor> actorKey, ComponentType component);
    void AddAndCloneComponentById(SlotmapKey<Actor> dstActorKey, SlotmapKey<Actor> srcActorKey, int id);

    template <typename ComponentType>
    void RemoveComponent(SlotmapKey<Actor> actorKey);
    void RemoveComponentById(SlotmapKey<Actor> actorKey, i32 id);
    void RemoveComponentById(Actor* actor, i32 id);

    void InitializeNewComponents();
    void ProcessActorsToRemove();

    template<typename ComponentType>
    void UpdateComponents(f32 dt);

    template<typename ComponentType>
    void LateUpdateComponents(f32 dt);

    template<typename ComponentType>
    void RenderComponents();
};


// template functions
template <typename ComponentType, i32 Count>
void ActorManager::AddComponentType()
{
    ASSERT(componentListMap.Count() + 1 <= componentTypeCount);

    void* buffer = MemoryManager::Get()->Alloc(sizeof(ComponentList<ComponentType>), memoryType);
    ComponentList<ComponentType>* componentList = new (buffer) ComponentList<ComponentType>();
    componentList->first = nullptr;
    componentList->allocator.Init(memoryType);
    componentListMap.Add(ComponentType::GetID(), (ComponentListBase*)componentList);
    maxComponentCount += Count;
}

template <typename ComponentType>
ComponentList<ComponentType>* ActorManager::GetComponents()
{
    return (ComponentList<ComponentType> *)(*componentListMap.Get(ComponentType::GetID()));
}


template <typename ComponentType>
void ActorManager::AddComponent(SlotmapKey<Actor> actorKey, ComponentType value)
{
    ComponentList<ComponentType>* list = (ComponentList<ComponentType> *)(*componentListMap.Get(ComponentType::GetID()));
    ComponentType* component = list->allocator.Alloc();
    *component = value;

    // TODO: insert at the end of the link list
    component->prev = nullptr;
    component->next = nullptr;
    if (list->first == nullptr)
    {
        list->first = component;
    }
    else
    {
        component->next = list->first;
        component->next->prev = component;
        list->first = component;
    }
    component->owner = actorKey;

    Actor* actor = GetActor(actorKey);
    actor->componentsMap.Add(ComponentType::GetID(), component);
    actor->componentsIds.Push(ComponentType::GetID());
    componentsToInit.Push(component);
}

template <typename ComponentType>
void ActorManager::RemoveComponent(SlotmapKey<Actor> actorKey)
{
    ComponentList<ComponentType>* list = (ComponentList<ComponentType> *)(*componentListMap.Get(ComponentType::GetID()));

    Actor* actor = GetActor(actorKey);

    ComponentType* component = actor->GetComponent<ComponentType>();
    component->OnTerminate(this);

    if (component->prev == nullptr)
    {
        list->first = component->next;
        if (list->first)
        {
            list->first->prev = nullptr;
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
        actor->componentsIds[actor->componentsIds.size - 1] = -1;
        actor->componentsIds.size--;
    }

    list->allocator.Free(component);
}


template<typename ComponentType>
void ActorManager::UpdateComponents(f32 dt)
{
    ComponentList<ComponentType> *list = GetComponents<ComponentType>();
    for (ComponentType *component = (ComponentType *)list->first;
        component != nullptr;
        component = (ComponentType*)component->next)
    {
        if (component->initialized && component->enable)
        {
            component->OnUpdate(this, dt);
        }
    }
}

template<typename ComponentType>
void ActorManager::LateUpdateComponents(f32 dt)
{
    ComponentList<ComponentType>* list = GetComponents<ComponentType>();
    for (ComponentType* component = (ComponentType*)list->first;
        component != nullptr;
        component = (ComponentType*)component->next)
    {
        if (component->initialized && component->enable)
        {
            component->OnLateUpdate(this, dt);
        }
    }
}

template<typename ComponentType>
void ActorManager::RenderComponents()
{
    ComponentList<ComponentType>* list = GetComponents<ComponentType>();
    for (ComponentType* component = (ComponentType*)list->first;
        component != nullptr;
        component = (ComponentType*)component->next)
    {
        if (component->initialized && component->enable)
        {
            component->OnRender(this);
        }
    }
}
