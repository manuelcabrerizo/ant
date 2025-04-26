#pragma once

struct TransformComponent : public Component<TransformComponent>
{
    vec3 position;
    vec3 scale;
    vec3 direction; // TODO: use quaternions for orientation
};