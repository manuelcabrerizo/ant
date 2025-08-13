#pragma once

#include "containers.h"

struct ComponentBase;

struct Actor
{
     HashMap<ComponentBase *> componentsMap;
     Array<i32> componentsIds;

     template<typename ComponentType>
     ComponentType* GetComponent()
     {
         ComponentType* component = (ComponentType *)*componentsMap.Get(ComponentType::GetID());
         return component;
     }
};