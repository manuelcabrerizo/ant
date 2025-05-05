#pragma once

#include "bone.h"

class Animation
{
private:
    f32 duration = 0.0f;
    i32 ticksPerSeconds = 0;
    Array<Bone> bones;
public:
    void Init(const char *filepath, i32 memoryType);
    f32 GetDuration() { return duration; }
    i32 GetTicksPerSeconds();
    Array<Bone>& GetBones();
};