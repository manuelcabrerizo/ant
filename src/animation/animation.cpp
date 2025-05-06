#include "animation.h"

// TODO: fix gImporter architecture
void Animation::Init(const char *filepath, i32 memoryType)
{
    const aiScene *scene = gImporter.ReadFile(filepath, aiProcess_MakeLeftHanded);
    ASSERT(scene);
    // Handle only one animation per file
    aiAnimation *animation = scene->mAnimations[0];
    duration = animation->mDuration;
    ticksPerSeconds = (i32)animation->mTicksPerSecond;
    bones.Init(animation->mNumChannels, memoryType);
    for(i32 i = 0; i < animation->mNumChannels; ++i)
    {
        Bone bone;
        bone.Init(i, animation->mChannels[i], memoryType);
        bones.Push(bone);
    }
}

i32 Animation::GetTicksPerSeconds()
{
    return ticksPerSeconds;
}

Array<Bone>& Animation::GetBones()
{
    return bones;
}