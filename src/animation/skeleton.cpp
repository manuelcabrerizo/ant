#include "skeleton.h"
#include "animation.h"

i32 Skeleton::Node::FindBoneIndex(aiBone **bones, i32 count, const char *name)
{
    for(i32 i = 0; i < count; ++i) 
    {
        aiBone *bone = bones[i];
        if(strncmp(bone->mName.C_Str(), name, strlen(name)) == 0) 
        {
            return i;
        }
    }
    return -1;
}

void Skeleton::Node::Init(aiNode *node, aiBone **bones, i32 count, i32 memoryType)
{
    boneIndex = FindBoneIndex(bones, count, node->mName.data);
    transformation = ai_mat4_to_sd_mat4(node->mTransformation);
    inverseBindPose = mat4(1.0f);
    if(boneIndex >= 0)
    {
        inverseBindPose = ai_mat4_to_sd_mat4(bones[boneIndex]->mOffsetMatrix);
    }
    if(node->mNumChildren > 0)
    {
        childrens.Init(node->mNumChildren, memoryType);
        for(i32 i = 0; i < node->mNumChildren; ++i) {
            Node children;
            children.Init(node->mChildren[i], bones, count, memoryType);
            childrens.Push(children);
        }
    }
}

void Skeleton::Init(const char *filepath, i32 memoryType)
{
    const aiScene *scene = gImporter.ReadFile(filepath, 0);
    ASSERT(scene);
    aiMesh *mesh = scene->mMeshes[0];
    root.Init(scene->mRootNode, mesh->mBones, mesh->mNumBones, memoryType);
    // init the pallete matrix to identity matrices
    for(i32 i = 0; i < 100; i++) {
        finalBoneMatrices[i] = mat4(1.0f);
    }
}

void Skeleton::Animate(Animation *animation, f32 dt)
{
    deltaTime = dt;
    currentTime[0] += animation->GetTicksPerSeconds() * dt;
    currentTime[0] = fmod(currentTime[0], animation->GetDuration());
    CalculateBoneTransform(animation, &root, mat4(1.0f));
}

void Skeleton::CalculateBoneTransform(Animation *animation, Node *node, mat4 parentTransform)
{
    mat4 nodeTransform = node->transformation;
    Bone *bone = node->boneIndex >= 0 ? &animation->GetBones()[node->boneIndex] : nullptr;
    mat4 globalTransform;
    if(bone)
    {
        bone->Update(currentTime[0]);
        nodeTransform = bone->GetLocalTransform();
        globalTransform = parentTransform * nodeTransform;
        finalBoneMatrices[bone->GetId()] = globalTransform * node->inverseBindPose;
    } else
    {
        globalTransform = parentTransform * nodeTransform;
    }

    for(i32 i = 0; i < node->childrens.size; ++i)
    {
        CalculateBoneTransform(animation, &node->childrens[i], globalTransform);
    }  
}