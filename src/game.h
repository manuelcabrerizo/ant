#pragma once

class Game
{
private:
     TextureManager tm;
     ModelManager mm;

     ActorManager am;

     InputSystem inputSystem;
     CameraSystem cameraSystem;
     RenderSystem renderSystem;
     WeaponSystem weaponSystem;

     Shader *shader;
         
     SlotmapKey<Actor> player;
     SlotmapKey<Actor> firstWeapon;
     SlotmapKey<Actor> secondWeapon;
     bool usingFirstWeapon = true;
     
public:     
     void Init();
     void Terminate();
     void Update(f32 dt);
     void Render();
};
