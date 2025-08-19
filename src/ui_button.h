#pragma once

#include <input_manager.h>
#include <ui_renderer.h>

enum class UIButtonState
{
    MouseOver,
    MouseOut,
    Clicked
};

template <typename Type>
class UIButton
{
private:
    Vector2 position;
    Vector2 size;
    const char* texture;
    UIButtonState state;
    void (Type::* OnClick)() = nullptr;
    Type* instance = nullptr;

public:

    void Init(const Vector2& position,
        const Vector2& size,
        const char* texture,
        Type* instance,
        void (Type::*OnClick)());
    void Update();
    void Render(UIRenderer& uiRenderer, int zIndex);
};

template <typename Type>
void UIButton<Type>::Init(
    const Vector2& position, const Vector2& size, const char *texture,
    Type* instance, void (Type::*OnClick)())
{
    this->position = position;
    this->size = size;
    this->texture = texture;
    this->instance = instance;
    this->OnClick = OnClick;
}

template <typename Type>
void UIButton<Type>::Update()
{
    int width, height;
    PlatformClientDimensions(&width, &height);

    int mouseX = InputManager::Get()->MouseX();
    int mouseY = height - InputManager::Get()->MouseY();

    if (mouseX >= position.x && mouseX <= position.x + size.x &&
        mouseY >= position.y && mouseY <= position.y + size.y)
    {
        state = UIButtonState::MouseOver;
        if (InputManager::Get()->MouseButtonJustDown(0))
        {
            state = UIButtonState::Clicked;
            if (instance && OnClick)
            {
                (instance->*OnClick)();
            }
        }
    }
    else
    {
        state = UIButtonState::MouseOut;
    }
}

template <typename Type>
void UIButton<Type>::Render(UIRenderer& uiRenderer, int zIndex)
{
    Vector2 renderPos = position + size * 0.5f;
    uiRenderer.DrawQuat(renderPos, size, zIndex, texture);
}