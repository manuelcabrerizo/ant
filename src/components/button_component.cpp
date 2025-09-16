#include "button_component.h"

bool ButtonComponent::IsSignal()
{
    return isSignal;
}

void ButtonComponent::SetSignal(bool value)
{
    isSignal = value;
}
