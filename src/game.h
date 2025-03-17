#pragma once

/*
=#=#=#=#=#=#=#=#=#=#
= TODO:
=#=#=#=#=#=#=#=#=#=#
= Run around on a level
= Multiple weapos and the avility to shoot them
= Collision detection with the scene
= Very simple AI for enemies
=#=#=#=#=#=#=#=#=#=#
*/

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

     CollisionWorld cw;
     
public:     
     void Init();
     void Terminate();
     void Update(f32 dt);
     void Render();
};
