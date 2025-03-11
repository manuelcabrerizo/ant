#pragma once

struct Actor;
class ActorManager;

struct TransformComponent
{
     vec3 position;
     vec3 scale;
     vec3 direction;
     SlotmapKey<Actor> owner;
};

struct RenderComponent
{
     Texture texture;
     Model model;
     SlotmapKey<Actor> owner;
};

struct InputComponent
{
     f32 yaw, pitch;
     SlotmapKey<Actor> owner;
};

struct WeaponComponent
{
     SlotmapKey<Actor> owner;
     SlotmapKey<Actor> weapon;
};

struct Actor
{
     SlotmapKey<TransformComponent> transform;
     SlotmapKey<RenderComponent> render;
     SlotmapKey<InputComponent> input;
     SlotmapKey<CameraComponent> camera;
     SlotmapKey<WeaponComponent> weapon;
};

class ActorManager
{
private:
     Slotmap<Actor> actors;
     Slotmap<TransformComponent> transformComponents;
     Slotmap<RenderComponent> renderComponents;
     Slotmap<InputComponent> inputComponents;
     Slotmap<CameraComponent> cameraComponents;
     Slotmap<WeaponComponent> weaponComponents;
     
public:
     void Init(i32 stackNum);
     void Terminate();

     SlotmapKey<Actor> CreateActor();
     void DestroyActor(SlotmapKey<Actor> actorKey);

     Actor *GetActor(SlotmapKey<Actor> actorKey);     
     Array<TransformComponent> *GetTransformComponents();
     Array<RenderComponent> *GetRenderComponents();
     Array<InputComponent> *GetInputComponents();
     Array<CameraComponent> *GetCameraComponents();
     Array<WeaponComponent> *GetWeaponComponents();
     
     void AddTransformComponent(SlotmapKey<Actor> actorKey,
                                vec3 position, vec3 scale, vec3 direction);
     void AddRenderComponent(SlotmapKey<Actor> actorKey, Model model, Texture texture);
     void AddInputComponent(SlotmapKey<Actor> actorKey);
     void AddCameraComponent(SlotmapKey<Actor> actorKey, vec3 pos, vec3 dir);
     void AddWeaponComponent(SlotmapKey<Actor> actorKey, SlotmapKey<Actor> weaponKey);

     void RemoveTransformComponent(SlotmapKey<Actor> actorKey);
     void RemoveRenderComponent(SlotmapKey<Actor> actorKey);
     void RemoveInputComponent(SlotmapKey<Actor> actorKey);
     void RemoveCameraComponent(SlotmapKey<Actor> actorKey);
     void RemoveWeaponComponent(SlotmapKey<Actor> actorKey);
     
     TransformComponent *GetTransformComponent(SlotmapKey<Actor> actorKey);
     RenderComponent *GetRenderComponent(SlotmapKey<Actor> actorKey);
     InputComponent *GetInputComponent(SlotmapKey<Actor> actorKey);
     CameraComponent *GetCameraComponent(SlotmapKey<Actor> actorKey);
     WeaponComponent *GetWeaponComponent(SlotmapKey<Actor> actorKey);

     void PrintActorAndCompoenentState();
};
