#ifndef COMPONENT_LIST_TPP
#define COMPONENT_LIST_TPP

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

#endif // COMPONENT_LIST_TPP