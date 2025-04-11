#pragma once

class Game
{
private:
     TextureManager textureManager;
     ModelManager modelManager;
     ActorManager actorManager;

     PlayerController playerController;
     CameraSystem cameraSystem;
     RenderSystem renderSystem;
     WeaponSystem weaponSystem;

     Shader *shader;
         
     SlotmapKey<Actor> player;
     SlotmapKey<Actor> firstWeapon;
     SlotmapKey<Actor> secondWeapon;
     bool usingFirstWeapon = true;

     CollisionWorld collisionWorld;
     
public:     
     void Init();
     void Terminate();
     void Update(f32 dt);
     void Render();
};
