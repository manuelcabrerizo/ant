#ifndef UI_BUTTON_TPP
#define UI_BUTTON_TPP

#include <input_manager.h>

template <typename Type>
void UIButton<Type>::Init(
    const Vector2& position, const Vector2& size, const char* texture[],
    Type* instance, void (Type::* OnClick)())
{
    this->position = position;
    this->size = size;
    this->texture[0] = texture[0];
    this->texture[1] = texture[1];
    this->texture[2] = texture[2];
    this->instance = instance;
    this->OnClick = OnClick;
}

template <typename Type>
void UIButton<Type>::Update()
{
    bool isActive = IsActive();
    if (isActive && !isClicked)
    {
        state = UIButtonState::MouseOver;
    }
    if (!isActive)
    {
        state = UIButtonState::MouseOut;
        isClicked = false;
    }
    if (state == UIButtonState::MouseOver && InputManager::Get()->MouseButtonJustDown(0))
    {
        isClicked = true;
        state = UIButtonState::Clicked;
    }
    if (state == UIButtonState::Clicked && InputManager::Get()->MouseButtonJustUp(0))
    {
        isClicked = false;
        state = UIButtonState::MouseOver;
        if (instance && OnClick)
        {
            (instance->*OnClick)();
        }
    }
}

template <typename Type>
void UIButton<Type>::Render(UIRenderer& uiRenderer, int zIndex)
{
    Vector2 renderPos = position + (size * 0.5f);
    uiRenderer.DrawQuat(renderPos, size, zIndex, texture[(unsigned int)state]);
}

template <typename Type>
bool UIButton<Type>::IsActive()
{
    int dpi = PlatformGetWindowDPI();
    int width, height;
    PlatformClientDimensions(&width, &height);

    int mouseX = InputManager::Get()->MouseX();
    int mouseY = height - InputManager::Get()->MouseY();

    // TODO: check when this is necesary
    float dpiFactor = 1.0f; // (float)dpi / 96.0f;

    int posX = (int)(position.x * dpiFactor);
    int posY = (int)(position.y * dpiFactor);
    int sizeX = (int)(size.x * dpiFactor);
    int sizeY = (int)(size.y * dpiFactor);

    return mouseX >= posX && mouseX <= posX + sizeX &&
        mouseY >= posY && mouseY <= posY + sizeY;
}

template <typename Type>
void UIButton<Type>::SetPosition(const Vector2& position)
{
    this->position = position;
}

template <typename Type>
void UIButton<Type>::SetSize(const Vector2& size)
{
    this->size = size;
}

#endif // UI_BUTTON_TPP