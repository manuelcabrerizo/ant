#pragma once

#include "component.h"
#include <math/vector3.h>

class TransformComponent : public Component<TransformComponent>
{
public:
    Vector3 position;
    Vector3 scale;
    Vector3 direction; // TODO: use quaternions for orientation
};