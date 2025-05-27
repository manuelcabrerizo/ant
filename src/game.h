#pragma once

#include <actor.h>
#include <asset_manager.h>

class Game
{
private:
     TextureManager textureManager;
     ModelManager modelManager;
     ShaderManager shaderManager;
     MaterialManager materialManager;
     ActorManager actorManager;
public:     
     void Init();
     void Terminate();
     void Update(f32 dt);
     void Render();
};
