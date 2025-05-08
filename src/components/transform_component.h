#pragma once

struct TransformComponent : public Component<TransformComponent>
{
    Vector3 position;
    Vector3 scale;
    Vector3 direction; // TODO: use quaternions for orientation
};