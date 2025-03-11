
void ActorManager::Init(i32 stackNum)
{
     actors.Init(100, stackNum);
     transformComponents.Init(100, stackNum);
     renderComponents.Init(100, stackNum);
     inputComponents.Init(1, stackNum);
     cameraComponents.Init(1, stackNum);
     weaponComponents.Init(100, stackNum);
}

void ActorManager::Terminate()
{
     actors.Clear();
     transformComponents.Clear();
     renderComponents.Clear();
     inputComponents.Clear();
     cameraComponents.Clear();
     weaponComponents.Clear();
}

SlotmapKey<Actor> ActorManager::CreateActor()
{
     Actor actor;
     return actors.Add(actor);
}

void ActorManager::DestroyActor(SlotmapKey<Actor> actorKey)
{
     Actor *actor = actors.Get(actorKey);
     if(actor->transform.gen != INVALID_KEY) transformComponents.Remove(actor->transform);
     if(actor->render.gen != INVALID_KEY) renderComponents.Remove(actor->render);
     if(actor->input.gen != INVALID_KEY) inputComponents.Remove(actor->input);
     if(actor->camera.gen != INVALID_KEY) cameraComponents.Remove(actor->camera);
     if(actor->weapon.gen != INVALID_KEY) weaponComponents.Remove(actor->weapon);
     actors.Remove(actorKey);
}

Actor *ActorManager::GetActor(SlotmapKey<Actor> actorKey)
{
     return actors.Get(actorKey);
}

Array<TransformComponent> *ActorManager::GetTransformComponents()
{
     return transformComponents.GetArray();
}

Array<RenderComponent> *ActorManager::GetRenderComponents()
{
     return renderComponents.GetArray();
}

Array<InputComponent> *ActorManager::GetInputComponents()
{
     return inputComponents.GetArray();
}

Array<CameraComponent> *ActorManager::GetCameraComponents()
{
     return cameraComponents.GetArray();
}

Array<WeaponComponent> *ActorManager::GetWeaponComponents()
{
     return weaponComponents.GetArray();
}

void ActorManager::AddTransformComponent(SlotmapKey<Actor> actorKey,
                                         vec3 position, vec3 scale, vec3 direction)
{    
     TransformComponent transform;
     transform.position = position;
     transform.scale = scale;
     transform.direction = direction;
     transform.owner = actorKey;
     SlotmapKey<TransformComponent> key = transformComponents.Add(transform);
     Actor *actor = GetActor(actorKey);
     actor->transform = key;
}

void ActorManager::AddRenderComponent(SlotmapKey<Actor> actorKey, Model model, Texture texture)
{
     RenderComponent render;
     render.model = model;
     render.texture = texture;
     render.owner = actorKey;
     SlotmapKey<RenderComponent> key = renderComponents.Add(render);
     Actor *actor = GetActor(actorKey);
     actor->render = key;
}

void ActorManager::AddInputComponent(SlotmapKey<Actor> actorKey)
{
     InputComponent input;
     input.owner = actorKey;
     SlotmapKey<InputComponent> key = inputComponents.Add(input);
     Actor *actor = GetActor(actorKey);
     actor->input = key;
}

void ActorManager::AddCameraComponent(SlotmapKey<Actor> actorKey, vec3 pos, vec3 dir)
{
     CameraComponent camera;
     camera.Init(pos, dir);
     camera.owner = actorKey;
     SlotmapKey<CameraComponent> key = cameraComponents.Add(camera);
     Actor *actor = GetActor(actorKey);
     actor->camera = key;
}

void ActorManager::AddWeaponComponent(SlotmapKey<Actor> actorKey, SlotmapKey<Actor> weaponKey)
{
     WeaponComponent weapon;
     weapon.weapon = weaponKey;
     weapon.owner = actorKey;
     SlotmapKey<WeaponComponent> key = weaponComponents.Add(weapon);
     Actor *actor = GetActor(actorKey);
     actor->weapon = key;
}

void ActorManager::RemoveTransformComponent(SlotmapKey<Actor> actorKey)
{
     Actor *actor = GetActor(actorKey);
     if(actor->transform.gen != INVALID_KEY) transformComponents.Remove(actor->transform);
}

void ActorManager::RemoveRenderComponent(SlotmapKey<Actor> actorKey)
{
     Actor *actor = GetActor(actorKey);
     if(actor->render.gen != INVALID_KEY) renderComponents.Remove(actor->render);
}

void ActorManager::RemoveInputComponent(SlotmapKey<Actor> actorKey)
{
     Actor *actor = GetActor(actorKey);;
     if(actor->input.gen != INVALID_KEY) inputComponents.Remove(actor->input);
}

void ActorManager::RemoveCameraComponent(SlotmapKey<Actor> actorKey)
{
     Actor *actor = GetActor(actorKey);
     if(actor->camera.gen != INVALID_KEY) cameraComponents.Remove(actor->camera);
}

void ActorManager::RemoveWeaponComponent(SlotmapKey<Actor> actorKey)
{
     Actor *actor = GetActor(actorKey);
     if(actor->weapon.gen != INVALID_KEY) weaponComponents.Remove(actor->weapon);
}

TransformComponent *ActorManager::GetTransformComponent(SlotmapKey<Actor> actorKey)
{
     Actor *actor = GetActor(actorKey);
     return transformComponents.Get(actor->transform);
}

RenderComponent *ActorManager::GetRenderComponent(SlotmapKey<Actor> actorKey)
{
     Actor *actor = GetActor(actorKey);
     return renderComponents.Get(actor->render);
}

InputComponent *ActorManager::GetInputComponent(SlotmapKey<Actor> actorKey)
{
     Actor *actor = GetActor(actorKey);
     return inputComponents.Get(actor->input);
}

CameraComponent *ActorManager::GetCameraComponent(SlotmapKey<Actor> actorKey)
{
     Actor *actor = GetActor(actorKey);
     return cameraComponents.Get(actor->camera);
}

WeaponComponent *ActorManager::GetWeaponComponent(SlotmapKey<Actor> actorKey)
{
     Actor *actor = GetActor(actorKey);
     return weaponComponents.Get(actor->weapon);
}


void ActorManager::PrintActorAndCompoenentState()
{
     printf("Actors count: %d\n", actors.Size());
     printf("transformComponents count: %d\n", transformComponents.Size());
     printf("renderComponents count: %d\n", renderComponents.Size());
     printf("inputComponents count: %d\n", inputComponents.Size());
     printf("cameraComponents count: %d\n", cameraComponents.Size());
     printf("weaponComponents count: %d\n", weaponComponents.Size());
}

