#pragma once

#include "component.h"
#include "../math/vector3.h"

class AnchorComponent : public Component<AnchorComponent>
{
public:
    Vector3 offset;
    Vector3 position;
};