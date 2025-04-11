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

void PlayerController::Update(ActorManager *am, CollisionWorld *cw, float dt)
{
     Array<PlayerControllerComponent>& playerControllers = am->GetPlayerControllerComponents();
     for(u32 i = 0; i < playerControllers.size; ++i)
     {
          SlotmapKey<Actor> actor = playerControllers[i].owner;

          playerController = &playerControllers[i];
          transform = am->GetTransformComponent(actor);
          camera = am->GetCameraComponent(actor);
          physics = am->GetPhysicsComponent(actor);
          moveDirection = vec3(0.0f);

          ProcessMouseMovement();
          ProcessKeyboardMovement();
          ProcessPhysics(dt);
          ProcessColisionDetectionAndResolution(cw);
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
          if(playerController->pitch > radians(89.0f))
          {
               playerController->pitch = radians(89.0f);
          }
          if(playerController->pitch < -radians(89.0f))
          {
               playerController->pitch = -radians(89.0f);
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
     if(playerController->grounded && InputManager::Get()->KeyJustDown(KEY_SPACE))
     {
          physics->velocity.y = 0.0f;
          physics->velocity += vec3(0.0f, 9.8f*1.5f, 0.0f);                    
     }
}

void PlayerController::ProcessPhysics(float dt)
{
     physics->forceAccumulator += vec3(0.0f, -9.8f*3.0f, 0.0f);

     vec3 movement = (moveDirection * speed);
     physics->acceleration = movement;

     physics->lastFrameAcceleration = physics->acceleration;
     physics->lastFrameAcceleration += physics->forceAccumulator;

     physics->velocity += physics->lastFrameAcceleration * dt;

     physics->velocity *= powf(0.01f, dt);

     transform->position += physics->velocity * dt;

     physics->forceAccumulator = vec3(0.0f);
}

void PlayerController::ProcessColisionDetectionAndResolution(CollisionWorld *cw)
{
     f32 colliderRadius = 0.2f;

     // Grounded test
     Segment groundSegment;
     vec3 colliderPos = transform->position + vec3(0.0f, -0.3f, 0.0f);
     groundSegment.Init(colliderPos,
          colliderPos - vec3(0.0f, colliderRadius + 0.05f, 0.0f));
     float tOut; vec3 nOut;
     playerController->grounded = cw->Intersect(groundSegment, tOut, nOut);

     // Colission detection and resolution
     Frame frame = MemoryManager::Get()->GetFrame();
     Array<CollisionData> collisionData;
     collisionData.Init(MAX_COLLISION_COUNT, FRAME_MEMORY);

     Sphere sphere;
     sphere.Init(transform->position + vec3(0.0f, -0.3f, 0.0f), colliderRadius);
     if(cw->Intersect(sphere, collisionData))
     {
          while(collisionData.size > 0)
          {
               vec3 n = collisionData[0].n;
               f32 penetration = collisionData[0].penetration;
               transform->position += penetration * n;
               transform->position += 0.001f * n;
               physics->acceleration = vec3(0.0f);
               physics->velocity -= dot(physics->velocity, n) * n;

               collisionData.Clear();

               sphere.Init(transform->position + vec3(0.0f, -0.3f, 0.0f), colliderRadius);
               cw->Intersect(sphere, collisionData);                    
          }    
     }
     MemoryManager::Get()->ReleaseFrame(frame);
}
