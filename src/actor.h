#pragma once

#include <utils.h>

class ActorManager;
class ComponentBase;

class Actor
{
    friend ActorManager;
private:
    int id;
    ActorManager* actorManager = nullptr;
public:
    template<typename ComponentType>
    ComponentType* GetComponent()
    {
        ASSERT(actorManager != nullptr);
        return actorManager->GetComponent<ComponentType>(this);
    }
};