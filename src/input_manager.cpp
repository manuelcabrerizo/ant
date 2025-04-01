InputManager InputManager::instance;
bool InputManager::initialize = false;

void InputManager::Init()
{
     if(initialize)
     {
          ASSERT(!"Error: input manager already initialize");
     }
     memset(&instance, 0, sizeof(InputManager));

     DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&instance.directInput, NULL);
     instance.directInput->CreateDevice(GUID_SysMouse, &instance.mouse, NULL);
     instance.mouse->SetDataFormat(&c_dfDIMouse2);
     instance.mouse->SetCooperativeLevel(*(HWND*)PlatformGetOsWindow(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
     instance.mouse->Acquire();
     printf("DirectInput8 Initialized!\n");
     
     initialize = true;
}

void InputManager::Terminate()
{
     if(!initialize)
     {
          ASSERT(!"Error: input manager has not been initialize");
     }

     if(instance.mouse) instance.mouse->Release();
     if(instance.directInput) instance.directInput->Release();
     printf("DirectInput8 Terminate!\n");
     
     initialize = false;
}

InputManager *InputManager::Get()
{
     if(!initialize)
     {
          ASSERT(!"Error: input manager has not been initialize");
     }
     return &instance;
}

void InputManager::Process()
{
     memcpy(keys[1], keys[0], KEY_COUNT * sizeof(bool));
     memcpy(mouseButtons[1], mouseButtons[0], MOUSE_BUTTON_COUNT * sizeof(bool));
     mousePosX[1] = mousePosX[0];
     mousePosY[1] = mousePosY[0];

     ZeroMemory(&mouseState, sizeof(mouseState));

     HRESULT hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState);
     if(FAILED(hr)) 
     {
          hr = mouse->Acquire();
          while(hr == DIERR_INPUTLOST)
          {
               printf("Mouse Device Lost!!!\n");
               hr = mouse->Acquire();
          }
     } 
}

bool InputManager::KeyDown(u32 key)
{
     return keys[0][key];
}

bool InputManager::KeyJustDown(u32 key)
{
     return keys[0][key] && !keys[1][key];
}

bool InputManager::KeyJustUp(u32 key)
{
     return !keys[0][key] && keys[1][key];
}

bool InputManager::MouseButtonDown(u32 button)
{
     return mouseButtons[0][button];
}

bool InputManager::MouseButtonJustDown(u32 button)
{
     return mouseButtons[0][button] && !mouseButtons[1][button];
}

bool InputManager::MouseButtonJustUp(u32 button)
{
     return !mouseButtons[0][button] && mouseButtons[1][button];
}

void InputManager::SetKey(u32 key, bool value)
{
     keys[0][key] = value;
}

void InputManager::SetMouseButton(u32 button, bool value)
{
     mouseButtons[0][button] = value;
}

void InputManager::SetMousePosition(i32 mouseX, i32 mouseY)
{
     mousePosX[0] = mouseX;
     mousePosY[0] = mouseY;
}

void InputManager::SetMouseLastPosition(i32 mouseX, i32 mouseY)
{
     mousePosX[1] = mouseX;
     mousePosY[1] = mouseY;
}

i32 InputManager::MouseX()
{
     return mousePosX[0];
}

i32 InputManager::MouseY()
{
     return mousePosY[0];
}

i32 InputManager::MouseXMovement()
{
     return mouseState.lX;
}

i32 InputManager::MouseYMovement()
{
     
     return mouseState.lY;
}
