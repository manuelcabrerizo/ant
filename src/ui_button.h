#pragma once

#include <input_manager.h>
#include <ui_renderer.h>

#include <windows.h>

enum class UIButtonState
{
    MouseOut = 0,
    MouseOver,
    Clicked
};

template <typename Type>
class UIButton
{
private:
    Vector2 position;
    Vector2 size;
    const char* texture[3];
    
    UIButtonState state;
    bool isClicked = false;

    void (Type::* OnClick)() = nullptr;
    Type* instance = nullptr;

    bool IsActive();

public:

    void Init(const Vector2& position,
        const Vector2& size,
        const char* texture[],
        Type* instance,
        void (Type::*OnClick)());
    void Update();
    void Render(UIRenderer& uiRenderer, int zIndex);
};

template <typename Type>
void UIButton<Type>::Init(
    const Vector2& position, const Vector2& size, const char *texture[],
    Type* instance, void (Type::*OnClick)())
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
    if(!isActive)
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
    Vector2 renderPos = position + size * 0.5f;
    uiRenderer.DrawQuat(renderPos, size, zIndex, texture[(unsigned int)state]);
}

template <typename Type>
bool UIButton<Type>::IsActive()
{
    int width, height;
    PlatformClientDimensions(&width, &height);
    int mouseX = InputManager::Get()->MouseX();
    int mouseY = height - InputManager::Get()->MouseY();
    return mouseX >= position.x && mouseX <= position.x + size.x &&
           mouseY >= position.y && mouseY <= position.y + size.y;
}
