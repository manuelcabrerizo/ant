void WeaponSystem::Init()
{

}

void WeaponSystem::Terminate()
{

}

void WeaponSystem::Update(ActorManager *am, f32 dt)
{
     Array<WeaponComponent> *weapons = am->GetWeaponComponents();
     for(u32 i = 0; i < weapons->size; ++i)
     {
          WeaponComponent *weapon = &(*weapons)[i];
          TransformComponent *transform = am->GetTransformComponent(weapon->owner);
          
          // TODO: make this more clear!
          TransformComponent *weaponTransform = am->GetTransformComponent(weapon->weapon);

          vec3 front = normalize(transform->direction);
          vec3 right = normalize(cross(vec3(0.0f, 1.0f, 0.0f), front));
          vec3 up = cross(front, right);
          weaponTransform->position = transform->position + front * 0.15f + right * 0.05f + up * -0.075f;
          weaponTransform->direction = transform->direction;
     }
}

void WeaponSystem::OnNotify(NotificationType type, Notification notification, void *sender)
{
     switch(type)
     {
     case NOTIFICATION_SHOOT:
     {
          printf((const char *)notification.data);
     } break;
     }
}
