#ifndef ACTOR_MANAGER_TPP
#define ACTOR_MANAGER_TPP

template <typename ComponentType, size_t Count>
void ActorManager::AddComponentType()
{
    ASSERT(componentListMap.Count() + 1 <= componentTypeCount);

    void* buffer = MemoryManager::Get()->Alloc(sizeof(ComponentList<ComponentType>), memoryType);
    ComponentList<ComponentType>* componentList = new (buffer) ComponentList<ComponentType>();
    componentList->Init(Count, memoryType);
    componentListMap.Add(ComponentType::GetID(), (ComponentListBase*)componentList);
    componentListToRemove.Push((ComponentListBase*)componentList);
    maxComponentCount += Count;
}

template <typename ComponentType>
ComponentList<ComponentType>* ActorManager::GetComponents()
{
    return (ComponentList<ComponentType> *)(*componentListMap.Get(ComponentType::GetID()));
}

template <typename ComponentType>
void ActorManager::AddComponent(Actor* actor, const ComponentType& value)
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
            component = (ComponentType*)components[i].component;
        }
    }
    return component;
}

template<typename ComponentType>
void ActorManager::UpdateComponents(f32 dt)
{
    ComponentList<ComponentType>* list = GetComponents<ComponentType>();
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
    ComponentType::OnSetRenderState();
    ComponentList<ComponentType>* list = GetComponents<ComponentType>();
    list->RenderComponents(this);
}

#endif // ACTOR_MANAGER_TPP
