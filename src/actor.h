#pragma once

#include <utils.h>

class ActorManager;
class ComponentBase;

enum class ActorTag
{
    Default = 0,
    Player,
    Enemy,
    Key,

    TagCount
};

class Actor
{
    friend ActorManager;
private:
    int id = 0;
    ActorTag tag = ActorTag::Default;
    ActorManager* actorManager = nullptr;
    bool isEnable = true;
public:
    static ActorTag TagNameToTag(const char* tagName);

    void SetTag(ActorTag tag);
    ActorTag GetTag();
    int GetID()
    {
        return id;
    }

    template<typename ComponentType>
    ComponentType* GetComponent();

    ActorManager* GetActorManager()
    {
        return actorManager;
    }

    bool IsEnable();
    void SetEnable(bool value);
};

template<typename ComponentType>
ComponentType* Actor::GetComponent()
{
    ASSERT(actorManager != nullptr);
    return actorManager->GetComponent<ComponentType>(this);
}