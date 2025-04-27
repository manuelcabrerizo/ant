void PlayerControllerComponent::OnInit(ActorManager *actorManager)
{
    speed = 20.0f;
    showMouse = false;
    PlatformShowMouse(showMouse);
    
    transform = actorManager->GetComponent<TransformComponent>(owner);
    camera = actorManager->GetComponent<CameraComponent>(owner);
    physics = actorManager->GetComponent<PhysicsComponent>(owner);
    weapon = actorManager->GetComponent<WeaponComponent>(owner);
}


void PlayerControllerComponent::OnTerminate(ActorManager *actorManager)
{
    showMouse = true;
    PlatformShowMouse(showMouse);
}

void PlayerControllerComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    ChangeWeapon(actorManager);
    ProcessMouseMovement();
    ProcessKeyboardMovement();
}

void PlayerControllerComponent::OnLateUpdate(ActorManager *actorManager, f32 dt)
{
    if(InputManager::Get()->MouseButtonJustDown(MOUSE_BUTTON_LEFT))
    {
         const char *message = "te dispare gato\n";
         NotificationManager::Get()->SendNotification(NOTIFICATION_SHOOT, (void *)message, strlen(message), (void *)this);
    }  

    if(InputManager::Get()->MouseButtonDown(MOUSE_BUTTON_LEFT))
    {
         int currentWeapon = usingFirstWeapon ? 0 : 1;
         AnchorComponent *weaponAnchor = actorManager->GetComponent<AnchorComponent>(weapons[currentWeapon]);
         vec3 targetPosition = camera->GetPosition() + camera->GetFront() * 100.0f;
         GraphicsManager::Get()->DebugDrawLine(weaponAnchor->position, targetPosition);
    }
}

void PlayerControllerComponent::ProcessMouseMovement()
{
    if(InputManager::Get()->KeyJustDown(KEY_ESCAPE))
    {
         showMouse = !showMouse;
         PlatformShowMouse(showMouse);
    }
    if(showMouse == false)
    {
         yaw += InputManager::Get()->MouseXMovement() * 0.001f;
         pitch += InputManager::Get()->MouseYMovement() * 0.001f;
         f32 limit = radians(89.0f);
         if(pitch > limit)
         {
              pitch = limit;
         }
         if(pitch < -limit)
         {
             pitch = -limit;
         }

         vec3 dir = vec3(0.0f, 0.0f, 1.0f);
         // TODO: this can be optimice by not using the generic angle axis rotation function
         transform->direction = rotate(mat4(1.0f), pitch, vec3(1, 0, 0)) * vec4(dir, 0.0f);
         transform->direction = rotate(mat4(1.0f), yaw, vec3(0, 1, 0)) * vec4(transform->direction, 0.0f);
         
         i32 windowX, windowY, windowW, windowH;
         PlaformGetWindowPos(&windowX, &windowY);
         PlatformClientDimensions(&windowW, &windowH);
         PlaformSetCursorPos(windowX + windowW/2, windowY + windowH/2); 
         InputManager::Get()->SetMousePosition(windowW/2, windowH/2);
         InputManager::Get()->SetMouseLastPosition(windowW/2, windowH/2);
    }
}

void PlayerControllerComponent::ProcessKeyboardMovement()
{
    vec3 moveDirection = vec3(0.0f);
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
         NotificationManager::Get()->SendNotification(NOTIFICATION_PLAYER_MOVE, (void *)&transform->position, sizeof(transform->position), (void *)this);
    }

    physics->acceleration = movement;
}

void PlayerControllerComponent::ChangeWeapon(ActorManager *actorManager)
{
    RenderComponent *weaponRenderComponents[2];
    weaponRenderComponents[0] = actorManager->GetComponent<RenderComponent>(weapons[0]);
    weaponRenderComponents[1] = actorManager->GetComponent<RenderComponent>(weapons[1]);

    if(InputManager::Get()->KeyJustDown(KEY_1) && !usingFirstWeapon)
    {
         weaponRenderComponents[1]->enable = false;
         weaponRenderComponents[0]->enable = true;    
         weapon->weapon = weapons[0];      
         usingFirstWeapon = true;
    }
    if(InputManager::Get()->KeyJustDown(KEY_2) && usingFirstWeapon)
    {
          weaponRenderComponents[0]->enable = false;   
          weaponRenderComponents[1]->enable = true;
          weapon->weapon = weapons[1];
         usingFirstWeapon = false;
    }
}