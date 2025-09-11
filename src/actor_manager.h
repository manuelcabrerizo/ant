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

    Array<Actor> actors;
    HashMap<StaticArray<ActorComponent, MAX_COMPONENTS_PER_ACTOR>> actorsComponentsMap;
    HashMap<ComponentListBase*> componentListMap;
    Array<ComponentBase*> componentsToInit;
    Array<ComponentListBase*> componentListToRemove;


public:
    void BeingInitialization(i32 actorCount, i32 componentTypeCount, i32 memoryType);
    void EndInitialization();
    void Terminate();

    template <typename ComponentType, size_t Count>
    void AddComponentType();

    template <typename ComponentType>
    ComponentList<ComponentType> *GetComponents();

    Actor *CreateActor(ActorTag tag = ActorTag::Default);
    Actor *CreateActorFromFile(const char* filepath);
    
    template <typename ComponentType>
    void AddComponent(Actor *actor, const ComponentType& component);

    template <typename ComponentType>
    void Clear();

    template <typename ComponentType>
    ComponentType* GetComponent(Actor* actor);

    void InitializeNewComponents();

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
    componentList->Init(Count, memoryType);
    componentListMap.Add(ComponentType::GetID(), (ComponentListBase*)componentList);
    componentListToRemove.Push((ComponentListBase *)componentList);
    maxComponentCount += Count;
}

template <typename ComponentType>
ComponentList<ComponentType>* ActorManager::GetComponents()
{
    return (ComponentList<ComponentType> *)(*componentListMap.Get(ComponentType::GetID()));
}

template <typename ComponentType>
void ActorManager::AddComponent(Actor *actor, const ComponentType& value)
{
    ComponentList<ComponentType>* list = (ComponentList<ComponentType> *)(*componentListMap.Get(ComponentType::GetID()));
    ComponentType* component = list->AddComponent(actor, value);

    auto& actorComponents = *actorsComponentsMap.Get(actor->id);
    actorComponents.Push({ ComponentType::GetID(), component });
    componentsToInit.Push(component);
}

template <typename ComponentType>
void ActorManager::Clear()
{
    ComponentList<ComponentType>* list = (ComponentList<ComponentType> *)(*componentListMap.Get(ComponentType::GetID()));
    list->Clear();
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
    list->UpdateComponents(this, dt);
}

template<typename ComponentType>
void ActorManager::LateUpdateComponents(f32 dt)
{
    ComponentList<ComponentType>* list = GetComponents<ComponentType>();
    list->LateUpdateComponents(this, dt);
}

template<typename ComponentType>
void ActorManager::RenderComponents()
{
    ComponentList<ComponentType>* list = GetComponents<ComponentType>();
    list->RenderComponents(this);
}
