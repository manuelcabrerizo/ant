#pragma once
#include "component.h"
#include "animation/skeleton.h"
#include "animation/animation.h"

class AnimationComponent : public Component<AnimationComponent>
{
public:
    Skeleton skeleton;
    Animation animation;
};