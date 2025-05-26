#include "animation.h"

#include <utils.h>
#include <assimp/postprocess.h>

// TODO: remove this
static i32 TempNextPower2(u32  x)
{
     int  value  =  1 ;
     while  ( value  <=  x)
     {
          value  =  value  <<  1 ;
     }
     return  value;
}

// TODO: remove model from this function
void Animation::Init(const char *filepath, Model *model, i32 memoryType)
{
    const aiScene *scene = Utils::importer.ReadFile(filepath, 
        aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    ASSERT(scene);
    // Handle only one animation per file
    aiAnimation *animation = scene->mAnimations[0];
    duration = animation->mDuration;
    ticksPerSeconds = (i32)animation->mTicksPerSecond;

    bonesInfo = model->GetBonesInfo();
    i32 boneCount = model->GetBoneCount();


    i32 channelsCount = animation->mNumChannels;
    if((channelsCount & (channelsCount - 1)) != 0)
    {
        channelsCount = TempNextPower2(channelsCount);
    }
    bones.Init(channelsCount, memoryType);
    for(i32 i = 0; i < animation->mNumChannels; ++i)
    {
        const char *boneName = animation->mChannels[i]->mNodeName.C_Str();
        if(bonesInfo.Contains(boneName) == false)
        {
            BoneInfo boneInfo;
            boneInfo.id = boneCount;
            boneInfo.offset = Matrix4(1.0f);
            bonesInfo.Add(boneName, boneInfo);
            boneCount++;
        }

        Bone bone;
        bone.Init(bonesInfo.Get(boneName)->id, animation->mChannels[i], memoryType);
        bones.Add(boneName, bone);
    }
}

i32 Animation::GetTicksPerSeconds()
{
    return ticksPerSeconds;
}

HashMap<Bone>& Animation::GetBones()
{
    return bones;
}

HashMap<BoneInfo>& Animation::GetBonesInfo()
{
    return bonesInfo;
}