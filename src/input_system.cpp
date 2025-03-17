void InputSystem::Init()
{

}

void InputSystem::Terminate()
{

}

void InputSystem::Update(ActorManager *am, CollisionWorld *cw, float dt)
{
     Array<InputComponent> *inputs = am->GetInputComponents();
     for(u32 i = 0; i < inputs->size; ++i)
     {
          SlotmapKey<Actor> actor = (*inputs)[i].owner;
          InputComponent *input = &(*inputs)[i];
          TransformComponent *transform = am->GetTransformComponent(actor);
          CameraComponent *camera = am->GetCameraComponent(actor);

          // TODO: implement some collision test
          vec3 playerVel = vec3(0.0f);
          if(InputManager::Get()->KeyDown(KEY_A))
          {
               playerVel -= camera->GetRight();
          }
          if(InputManager::Get()->KeyDown(KEY_D))
          {
               playerVel += camera->GetRight();
          }
          if(InputManager::Get()->KeyDown(KEY_W))
          {
               playerVel += camera->GetFront();
          }
          if(InputManager::Get()->KeyDown(KEY_S))
          {
               playerVel -= camera->GetFront();
          }
          if(dot(playerVel, playerVel) > 0.0f)
          {
               playerVel = normalize(playerVel);
          }

          // Collision detection, this should be done in its own system
          vec3 playerNewPosition = transform->position;
          playerNewPosition += (playerVel * 2.0f) * dt;

          Segment playerMovement;
          playerMovement.Init(transform->position, playerNewPosition);

          f32 t;
          vec3 n;
          if(cw->Intersect(playerMovement, t, n))
          {
               vec3 movement = playerNewPosition - transform->position;
               transform->position += (movement * t) + (n * 0.001f);
               f32 projection = dot(movement, n);
               vec3 newVelocity = (movement - (n * projection)) * (1.0f - t);
               transform->position += newVelocity;
          }
          else
          {
               transform->position = playerNewPosition;
          }

          
          if(InputManager::Get()->MouseButtonJustDown(MOUSE_BUTTON_RIGHT))
          {
               PlatformShowMouse(false);
          }
          if(InputManager::Get()->MouseButtonJustUp(MOUSE_BUTTON_RIGHT))
          {
               PlatformShowMouse(true);
          }
          
          if(InputManager::Get()->MouseButtonDown(MOUSE_BUTTON_RIGHT))
          {
               input->yaw += InputManager::Get()->MouseXMovement() * 0.0016f;
               input->pitch += InputManager::Get()->MouseYMovement() * 0.0016f;
               if(input->pitch > radians(89.0f))
               {
                    input->pitch = radians(89.0f);
               }
               if(input->pitch < -radians(89.0f))
               {
                    input->pitch = -radians(89.0f);
               }

               vec3 dir = vec3(0.0f, 0.0f, 1.0f);
               transform->direction = rotate(mat4(1.0f), input->pitch, vec3(1, 0, 0)) * vec4(dir, 0.0f);
               transform->direction = rotate(mat4(1.0f), input->yaw, vec3(0, 1, 0)) * vec4(transform->direction, 0.0f);

               i32 windowX, windowY, windowW, windowH;
               PlaformGetWindowPos(&windowX, &windowY);
               PlatformClientDimensions(&windowW, &windowH);
               PlaformSetCursorPos(windowX + (windowW/2), windowY + (windowH/2)); 
               InputManager::Get()->SetMousePosition(windowW/2, windowH/2);
               InputManager::Get()->SetMouseLastPosition(windowW/2, windowH/2);
          }
          
          if(InputManager::Get()->MouseButtonJustDown(MOUSE_BUTTON_LEFT))
          {
               Notification notification = {};
               const char *message = "te dispare gato\n";
               memcpy(notification.data, message, strlen(message));
               NotificationManager::Get()->SendNotification(notification, NOTIFICATION_SHOOT, (void *)this);
          }  
     }
}
