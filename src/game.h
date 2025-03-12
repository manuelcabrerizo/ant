#pragma once

class Game
{
private:
     ActorManager am;

     InputSystem inputSystem;
     CameraSystem cameraSystem;
     RenderSystem renderSystem;
     WeaponSystem weaponSystem;

     Model cube;
     Model plane;
     Model pistol;
     Model sniper;
     Model warrior;
     Model wall;

     Shader *shader;
     
     Texture *texture;
     Texture *warriorTexture;

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
