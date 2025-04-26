void WeaponSystem::Init()
{

}

void WeaponSystem::Terminate()
{

}

void WeaponSystem::Update(ActorManager *am, f32 dt)
{
     Array<WeaponComponent>& weapons = am->GetComponents<WeaponComponent>();
     for(u32 i = 0; i < weapons.size; ++i)
     {
          WeaponComponent *weapon = &weapons[i];
          TransformComponent *transform = am->GetComponent<TransformComponent>(weapon->owner);
          
          // TODO: make this more clear!
          TransformComponent *weaponTransform = am->GetComponent<TransformComponent>(weapon->weapon);
          AnchorComponent *weaponAnchor = am->GetComponent<AnchorComponent>(weapon->weapon);

          vec3 front = normalize(transform->direction);
          vec3 right = normalize(cross(vec3(0.0f, 1.0f, 0.0f), front));
          vec3 up = cross(front, right);
          weaponTransform->position = transform->position + front * 0.15f + right * 0.05f + up * -0.075f;
          weaponTransform->direction = transform->direction;
          
          weaponAnchor->position = weaponTransform->position +
               right * weaponAnchor->offset.x +
               up * weaponAnchor->offset.y +
               front * weaponAnchor->offset.z;
     }
}

void WeaponSystem::OnNotify(NotificationType type, void *data, size_t size, void *sender)
{
     switch(type)
     {
     case NOTIFICATION_SHOOT:
     {
          printf((const char *)data);
     } break;
     }
}
