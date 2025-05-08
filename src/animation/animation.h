#pragma once

#include "bone.h"

class Animation
{
private:
    f32 duration = 0.0f;
    i32 ticksPerSeconds = 0;
    HashMap<Bone> bones;
    HashMap<BoneInfo> bonesInfo;
public:
    void Init(const char *filepath, Model *model, i32 memoryType);
    f32 GetDuration() { return duration; }
    i32 GetTicksPerSeconds();
    HashMap<Bone>& GetBones();
    HashMap<BoneInfo>& GetBonesInfo();
};