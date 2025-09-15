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

#include "actor_manager.tpp"

