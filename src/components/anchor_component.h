#pragma once

struct AnchorComponent : public Component<AnchorComponent>
{
    vec3 offset;
    vec3 position;
};