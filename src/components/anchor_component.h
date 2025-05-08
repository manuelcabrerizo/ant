#pragma once

class AnchorComponent : public Component<AnchorComponent>
{
public:
    Vector3 offset;
    Vector3 position;
};