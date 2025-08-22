#pragma once

#include <utils.h>

class ActorManager;
class ComponentBase;

enum class ActorTag
{
    Default = 0,
    Player,
    Enemy,

    TagCount
};

class Actor
{
    friend ActorManager;
private:
    int id = 0;
    ActorTag tag = ActorTag::Default;
    ActorManager* actorManager = nullptr;
public:
    static ActorTag TagNameToTag(const char* tagName);

    void SetTag(ActorTag tag);
    ActorTag GetTag();

    template<typename ComponentType>
    ComponentType* GetComponent();
};

template<typename ComponentType>
ComponentType* Actor::GetComponent()
{
    ASSERT(actorManager != nullptr);
    return actorManager->GetComponent<ComponentType>(this);
}