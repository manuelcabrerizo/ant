#pragma once

#include <actor.h>
#include <asset_manager.h>

class Game
{
private:
     TextureManager textureManager;
     ModelManager modelManager;
     VertexShaderManager vertexShaderManager;
     FragmentShaderManager fragmentShaderManager;
     MaterialManager materialManager;
     ActorManager actorManager;
public:     
     void Init();
     void Terminate();
     void Update(f32 dt);
     void Render();
};
