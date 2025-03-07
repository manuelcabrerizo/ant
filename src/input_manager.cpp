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

void InputManager::Process()
{
     memcpy(keys[1], keys[0], KEY_COUNT * sizeof(bool));
     memcpy(mouseButtons[1], mouseButtons[0], MOUSE_BUTTON_COUNT * sizeof(bool));
     mousePosX[1] = mousePosX[0];
     mousePosY[1] = mousePosY[0];
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
     return mousePosX[0] - mousePosX[1];
}

i32 InputManager::MouseYMovement()
{
     
     return mousePosY[0] - mousePosY[1];
}
