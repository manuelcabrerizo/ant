#pragma once

#include <containers.h>
#include <actor.h>
#include <component_list.h>

#define MAX_COMPONENTS_PER_ACTOR 32

struct ActorComponent
{
    int id;
    ComponentBase* component;
};

class ActorManager
{
private:
    static int actorGeneration;

    i32 memoryType;
    i32 componentTypeCount = 0;
    i32 maxActorCount = 0;
    i32 maxComponentCount = 0;

    BlockAllocator<sizeof(Actor)> allocator;
    Array<Actor*> toRemove;

    HashMap<StaticArray<ActorComponent, MAX_COMPONENTS_PER_ACTOR>> actorsComponentsMap;
    HashMap<ComponentListBase*> componentListMap;

    Array<ComponentBase*> componentsToInit;

public:
    void BeingInitialization(i32 actorCount, i32 componentTypeCount, i32 memoryType);
    void EndInitialization();

    void Terminate();
    void Clear();

    template <typename ComponentType, size_t Count>
    void AddComponentType();
    template <typename ComponentType>
    ComponentList<ComponentType> *GetComponents();

    Actor *CreateActor(ActorTag tag = ActorTag::Default);
    Actor *CreateActorFromFile(const char* filepath);
    Actor *CloneActor(Actor *srcActor);
    void DestroyActor(Actor *actor);
    
    template <typename ComponentType>
    void AddComponent(Actor *actor, ComponentType component);
    void AddAndCloneComponent(Actor* dstActor, Actor* srcActor, ActorComponent actorComponent);

    template <typename ComponentType>
    void RemoveComponent(Actor *ctor);
    void RemoveActorComponent(Actor* actor, ActorComponent actorComponent);

    template <typename ComponentType>
    ComponentType* GetComponent(Actor* actor);

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
template <typename ComponentType, size_t Count>
void ActorManager::AddComponentType()
{
    ASSERT(componentListMap.Count() + 1 <= componentTypeCount);

    void* buffer = MemoryManager::Get()->Alloc(sizeof(ComponentList<ComponentType>), memoryType);
    ComponentList<ComponentType>* componentList = new (buffer) ComponentList<ComponentType>();
    componentList->allocator.Init(Count, memoryType);
    componentListMap.Add(ComponentType::GetID(), (ComponentListBase*)componentList);
    maxComponentCount += Count;
}

template <typename ComponentType>
ComponentList<ComponentType>* ActorManager::GetComponents()
{
    return (ComponentList<ComponentType> *)(*componentListMap.Get(ComponentType::GetID()));
}


template <typename ComponentType>
void ActorManager::AddComponent(Actor *actor, ComponentType value)
{
    ComponentList<ComponentType>* list = (ComponentList<ComponentType> *)(*componentListMap.Get(ComponentType::GetID()));
    void *buffer = list->allocator.Alloc();
    ComponentType* component = new (buffer) ComponentType();
    *component = value;
    component->owner = actor;

    auto& components = *actorsComponentsMap.Get(actor->id);
    components.Push({ ComponentType::GetID(), component });
    
    componentsToInit.Push(component);
}

template <typename ComponentType>
void ActorManager::RemoveComponent(Actor *actor)
{
    ComponentList<ComponentType>* list = (ComponentList<ComponentType> *)(*componentListMap.Get(ComponentType::GetID()));

    ComponentType* component = actor->GetComponent<ComponentType>();
    component->OnTerminate(this);

    auto& components = *actorsComponentsMap.Get(actor->id);
    i32 foundIndex = -1;
    for (i32 i = components.size - 1; i >= 0; --i)
    {
        if (components[i].id == ComponentType::GetID())
        {
            foundIndex = i;
        }
    }

    if (foundIndex >= 0)
    {
        components[foundIndex] = components[components.size - 1];
        components[components.size - 1] = {};
        components.size--;
    }

    list->allocator.Free(component);
}

template <typename ComponentType>
ComponentType* ActorManager::GetComponent(Actor* actor)
{
    // TODO: Linear search for now
    ComponentType* component = nullptr;
    auto& components = *actorsComponentsMap.Get(actor->id);
    for (int i = 0; i < components.size; ++i)
    {
        if (ComponentType::GetID() == components[i].id)
        {
            component = (ComponentType *)components[i].component;
        }
    }
    return component;
}

template<typename ComponentType>
void ActorManager::UpdateComponents(f32 dt)
{
    ComponentList<ComponentType> *list = GetComponents<ComponentType>();
    auto componentArray = list->allocator.GetBlockArray();
    size_t size = list->allocator.GetBlockCount();
    size_t used = list->allocator.GetBlockUsed();

    size_t usedFound = 0;
    for (int i = 0; i < size; ++i)
    {
        if (!componentArray[i].header.occupied)
        {
            continue;
        }
        usedFound++;

        ComponentType* component = (ComponentType*)&componentArray[i].data[0];
        if (component->initialized && component->enable)
        {
            component->OnUpdate(this, dt);
        }

        if (usedFound == used)
        {
            break;
        }
    }
}

template<typename ComponentType>
void ActorManager::LateUpdateComponents(f32 dt)
{
    ComponentList<ComponentType>* list = GetComponents<ComponentType>();
    auto componentArray = list->allocator.GetBlockArray();
    size_t size = list->allocator.GetBlockCount();
    size_t used = list->allocator.GetBlockUsed();

    size_t usedFound = 0;
    for (int i = 0; i < size; ++i)
    {
        if (!componentArray[i].header.occupied)
        {
            continue;
        }
        usedFound++;

        ComponentType* component = (ComponentType*)&componentArray[i].data[0];
            if (component->initialized && component->enable)
            {
                component->OnLateUpdate(this, dt);
            }

        if (usedFound == used)
        {
            break;
        }
    }
}

template<typename ComponentType>
void ActorManager::RenderComponents()
{
    ComponentList<ComponentType>* list = GetComponents<ComponentType>();
    auto componentArray = list->allocator.GetBlockArray();
    size_t size = list->allocator.GetBlockCount();
    size_t used = list->allocator.GetBlockUsed();

    size_t usedFound = 0;
    for (int i = 0; i < size; ++i)
    {
        if (!componentArray[i].header.occupied)
        {
            continue;
        }
        usedFound++;

        ComponentType* component = (ComponentType*)&componentArray[i].data[0];
            if (component->initialized && component->enable)
            {
                component->OnRender(this);
            }

        if (usedFound == used)
        {
            break;
        }
    }
}
