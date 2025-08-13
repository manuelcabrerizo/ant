#pragma once

#include <actor_manager.h>
#include <scene.h>

class Game
{
private:
    Array<Scene> scenes;
    ActorManager actorManager;
public:     
    void Init();
    void Terminate();
    void Update(f32 dt);
    void Render(f32 dt);
};
