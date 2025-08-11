#pragma once

class ActorManager;

class Scene
{
    ActorManager* actorManager = nullptr;
public:
    void Load(ActorManager* actorManager, const char *filepath);
    void Unload();
};

