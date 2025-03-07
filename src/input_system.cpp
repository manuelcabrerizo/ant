void InputSystem::Init()
{

}

void InputSystem::Terminate()
{

}

void InputSystem::Update(InputManager *im, ActorManager *am, float dt)
{
     Array<InputComponent> *inputs = am->GetInputComponents();
     for(u32 i = 0; i < inputs->size; ++i)
     {
          SlotmapKey<Actor> actor = (*inputs)[i].owner;
          InputComponent *input = &(*inputs)[i];
          TransformComponent *transform = am->GetTransformComponent(actor);
          CameraComponent *camera = am->GetCameraComponent(actor);

          vec3 playerVel = vec3(0.0f);
          if(im->KeyDown(KEY_A))
          {
               playerVel -= camera->GetRight();
          }
          if(im->KeyDown(KEY_D))
          {
               playerVel += camera->GetRight();
          }
          if(im->KeyDown(KEY_W))
          {
               playerVel += camera->GetWorldFront();
          }
          if(im->KeyDown(KEY_S))
          {
               playerVel -= camera->GetWorldFront();
          }
          if(dot(playerVel, playerVel) > 0.0f)
          {
               playerVel = normalize(playerVel);
          }
          transform->position += playerVel * dt;

          if(im->MouseButtonJustDown(MOUSE_BUTTON_RIGHT))
          {
               PlatformShowMouse(false);
          }
          if(im->MouseButtonJustUp(MOUSE_BUTTON_RIGHT))
          {
               PlatformShowMouse(true);
          }
          
          if(im->MouseButtonDown(MOUSE_BUTTON_RIGHT))
          {
               input->yaw += im->MouseXMovement() * 0.0016f;
               input->pitch += im->MouseYMovement() * 0.0016f;
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
               im->SetMousePosition(windowW/2, windowH/2);
               im->SetMouseLastPosition(windowW/2, windowH/2);
          }          
     }
}
