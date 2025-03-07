#pragma once

class Game
{
private:
     Arena gameArena;

     ActorManager am;

     InputSystem inputSystem;
     CameraSystem cameraSystem;
     RenderSystem renderSystem;
     WeaponSystem weaponSystem;

     Model cube;
     Model plane;
     Model pistol;
     Model sniper;

     Shader shader;
     Texture texture;

     SlotmapKey<Actor> player;
     SlotmapKey<Actor> firstWeapon;
     SlotmapKey<Actor> secondWeapon;
     bool usingFirstWeapon = true;
     
public:     
     void Init();
     void Terminate();
     void Update(InputManager *input, f32 dt);
     void Render();
};
