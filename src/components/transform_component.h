#pragma once

#include "component.h"
#include <math/vector3.h>

class TransformComponent : public Component<TransformComponent>
{
public:
    Vector3 position;
    Vector3 scale;
    Vector3 rotation; // Euler angles

    // TODO: add front right and up vectors for gameplay
    //Vector3 direction;
};