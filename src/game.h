#pragma once

class Game
{
private:
     TextureManager textureManager;
     ModelManager modelManager;
     ShaderManager shaderManager;
     ActorManager actorManager; 
public:     
     void Init();
     void Terminate();
     void Update(f32 dt);
     void Render();
};
