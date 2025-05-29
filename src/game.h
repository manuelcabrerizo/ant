#pragma once

#include <actor.h>

class Game
{
private:
    ActorManager actorManager;
public:     
    void Init();
    void Terminate();
    void Update(f32 dt);
    void Render();
};
