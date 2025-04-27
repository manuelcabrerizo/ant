#pragma once

class AnchorComponent : public Component<AnchorComponent>
{
public:
    vec3 offset;
    vec3 position;
};