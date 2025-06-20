#pragma once

#include <actor.h>

class Game
{
private:
    ActorManager actorManager;
    SlotmapKey<Actor> player;
public:     
    void Init();
    void Terminate();
    void Update(f32 dt);
    void Render(f32 dt);
};
