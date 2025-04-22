#pragma once

class Game
{
private:
     TextureManager textureManager;
     ModelManager modelManager;
     ShaderManager shaderManager;
     ActorManager actorManager;

     PlayerController playerController;
     PhysicsSystem physicsSystem;
     CameraSystem cameraSystem;
     RenderSystem renderSystem;
     WeaponSystem weaponSystem;
     EnemySystem enemySystem;    
public:     
     void Init();
     void Terminate();
     void Update(f32 dt);
     void Render();
};
