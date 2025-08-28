#pragma once

#include <math/vector3.h>
#include <math/matrix4.h>
#include <math/quaternion.h>

class ActorManager;

class Scene
{
private:
    ActorManager* actorManager = nullptr;
public:
    void Load(ActorManager* actorManager, const char *filepath);
    void Unload();
};

