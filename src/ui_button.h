#pragma once

#include <ui_renderer.h>

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

    void SetPosition(const Vector2& position);
    void SetSize(const Vector2& size);
};

#include "ui_button.tpp"