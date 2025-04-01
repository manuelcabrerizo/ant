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
               playerVel += camera->GetWorldFront();
          }
          if(InputManager::Get()->KeyDown(KEY_S))
          {
               playerVel -= camera->GetWorldFront();
          }

          if(InputManager::Get()->KeyDown(KEY_R))
          {
               playerVel += camera->GetWorldUp();
          }
          if(InputManager::Get()->KeyDown(KEY_F))
          {
               playerVel -= camera->GetWorldUp();
          }
          
          if(dot(playerVel, playerVel) > 0.0f)
          {
               playerVel = normalize(playerVel);
          }

          // Collision detection, this should be done in its own system
          vec3 movement = (playerVel * 2.0f) * dt;

          Frame frame = MemoryManager::Get()->GetFrame(FRAME_MEMORY);

          Array<CollisionData> collisionData;
          collisionData.Init(MAX_COLLISION_COUNT, FRAME_MEMORY);
          
          transform->position += movement;
          
          Sphere sphere;
          sphere.Init(transform->position + vec3(0.0f, -0.3f, 0.0f), 0.2f);
          if(cw->Intersect(sphere, collisionData))
          {
               while(collisionData.size > 0)
               {
                    vec3 n = collisionData[i].n;
                    f32 penetration = collisionData[i].penetration;
                    transform->position += penetration * n;
                    transform->position += 0.001f * n;

                    collisionData.Clear();

                    sphere.Init(transform->position + vec3(0.0f, -0.3f, 0.0f), 0.2f);
                    cw->Intersect(sphere, collisionData);                    
               }    
          }

          MemoryManager::Get()->ReleaseFrame(frame);


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
               // TODO: see if the dt in the input position integration is correct
               input->yaw += InputManager::Get()->MouseXMovement() * 0.001f;
               input->pitch += InputManager::Get()->MouseYMovement() * 0.001f;
               if(input->pitch > radians(89.0f))
               {
                    input->pitch = radians(89.0f);
               }
               if(input->pitch < -radians(89.0f))
               {
                    input->pitch = -radians(89.0f);
               }

               vec3 dir = vec3(0.0f, 0.0f, 1.0f);
               // TODO: this can be optimice by not using the generic angle axis rotation function
               transform->direction = rotate(mat4(1.0f), input->pitch, vec3(1, 0, 0)) * vec4(dir, 0.0f);
               transform->direction = rotate(mat4(1.0f), input->yaw, vec3(0, 1, 0)) * vec4(transform->direction, 0.0f);
               
               i32 windowX, windowY, windowW, windowH;
               PlaformGetWindowPos(&windowX, &windowY);
               PlatformClientDimensions(&windowW, &windowH);

               PlaformSetCursorPos(windowX + windowW/2, windowY + windowH/2); 
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
