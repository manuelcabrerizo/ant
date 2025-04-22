void PlayerController::Init()
{
     speed = 20.0f;
     showMouse = false;
     PlatformShowMouse(showMouse);
}

void PlayerController::Terminate()
{
     showMouse = true;
     PlatformShowMouse(showMouse);
}

void PlayerController::Update(ActorManager *am, f32 dt)
{
     Array<PlayerControllerComponent>& playerControllers = am->GetComponents<PlayerControllerComponent>();
     for(u32 i = 0; i < playerControllers.size; ++i)
     {
          SlotmapKey<Actor> actor = playerControllers[i].owner;

          playerController = &playerControllers[i];

          if(!playerController->enable)
          {
               continue;
          }

          transform = am->GetComponent<TransformComponent>(actor);
          camera = am->GetComponent<CameraComponent>(actor);
          physics = am->GetComponent<PhysicsComponent>(actor);
          moveDirection = vec3(0.0f);

          ChangeWeapon(am);

          ProcessMouseMovement();
          ProcessKeyboardMovement();
     }
}

void PlayerController::ProcessMouseMovement()
{
     if(InputManager::Get()->KeyJustDown(KEY_ESCAPE))
     {
          showMouse = !showMouse;
          PlatformShowMouse(showMouse);
     }
     if(showMouse == false)
     {
          playerController->yaw += InputManager::Get()->MouseXMovement() * 0.001f;
          playerController->pitch += InputManager::Get()->MouseYMovement() * 0.001f;
          f32 limit = radians(89.0f);
          if(playerController->pitch > limit)
          {
               playerController->pitch = limit;
          }
          if(playerController->pitch < -limit)
          {
               playerController->pitch = -limit;
          }

          vec3 dir = vec3(0.0f, 0.0f, 1.0f);
          // TODO: this can be optimice by not using the generic angle axis rotation function
          transform->direction = rotate(mat4(1.0f), playerController->pitch, vec3(1, 0, 0)) * vec4(dir, 0.0f);
          transform->direction = rotate(mat4(1.0f), playerController->yaw, vec3(0, 1, 0)) * vec4(transform->direction, 0.0f);
          
          i32 windowX, windowY, windowW, windowH;
          PlaformGetWindowPos(&windowX, &windowY);
          PlatformClientDimensions(&windowW, &windowH);
          PlaformSetCursorPos(windowX + windowW/2, windowY + windowH/2); 
          InputManager::Get()->SetMousePosition(windowW/2, windowH/2);
          InputManager::Get()->SetMouseLastPosition(windowW/2, windowH/2);

          if(InputManager::Get()->MouseButtonJustDown(MOUSE_BUTTON_LEFT))
          {
               Notification notification = {};
               const char *message = "te dispare gato\n";
               memcpy(notification.data, message, strlen(message));
               NotificationManager::Get()->SendNotification(notification, NOTIFICATION_SHOOT, (void *)this);
          }  
     }
}

void PlayerController::ProcessKeyboardMovement()
{
     if(InputManager::Get()->KeyDown(KEY_A))
     {
          moveDirection -= camera->GetRight();
     }
     if(InputManager::Get()->KeyDown(KEY_D))
     {
          moveDirection += camera->GetRight();
     }
     if(InputManager::Get()->KeyDown(KEY_W))
     {
          moveDirection += camera->GetWorldFront();
     }
     if(InputManager::Get()->KeyDown(KEY_S))
     {
          moveDirection -= camera->GetWorldFront();
     }
     
     if(dot(moveDirection, moveDirection) > 0.0f)
     {
          moveDirection = normalize(moveDirection);
     }

     // Jump code
     if(physics->grounded && InputManager::Get()->KeyJustDown(KEY_SPACE))
     {
          physics->velocity.y = 0.0f;
          physics->velocity += vec3(0.0f, 9.8f*1.5f, 0.0f);                    
     }

     vec3 movement = (moveDirection * speed);

     if(dot(movement, movement) > 0.0f)
     {
          Notification notification;
          vec3 *position = (vec3 *)notification.data;
          *position = transform->position;
          NotificationManager::Get()->SendNotification(notification, NOTIFICATION_PLAYER_MOVE, (void *)this);
     }

     physics->acceleration = movement;
}

void PlayerController::ChangeWeapon(ActorManager *actorManager)
{     
     RenderComponent *weaponRenderComponents[2];
     weaponRenderComponents[0] = actorManager->GetComponent<RenderComponent>(playerController->weapons[0]);
     weaponRenderComponents[1] = actorManager->GetComponent<RenderComponent>(playerController->weapons[1]);

     if(InputManager::Get()->KeyJustDown(KEY_1) && !playerController->usingFirstWeapon)
     {
          weaponRenderComponents[1]->enable = false;
          weaponRenderComponents[0]->enable = true;          
          
          actorManager->RemoveComponent<WeaponComponent>(playerController->owner);
          WeaponComponent weapon;
          weapon.weapon = playerController->weapons[0];
          actorManager->AddComponent<WeaponComponent>(playerController->owner, weapon);

          playerController->usingFirstWeapon = true;
     }
     if(InputManager::Get()->KeyJustDown(KEY_2) && playerController->usingFirstWeapon)
     {
          weaponRenderComponents[0]->enable = false;   
          weaponRenderComponents[1]->enable = true;
           
          actorManager->RemoveComponent<WeaponComponent>(playerController->owner);
          WeaponComponent weapon;
          weapon.weapon = playerController->weapons[1];
          actorManager->AddComponent<WeaponComponent>(playerController->owner, weapon);

          playerController->usingFirstWeapon = false;
     }
}
