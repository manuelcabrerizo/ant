#pragma once

#include "component.h"
#include <interfaces\ISignable.h>

class ButtonComponent : public Component<ButtonComponent>, public ISignable
{
private:
    bool isSignal = false;
public:
    bool IsSignal() override;
    void SetSignal(bool value);
};