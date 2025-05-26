#include "skeleton.h"
#include "animation.h"
#include <utils.h>

void Skeleton::Node::Init(aiNode *node, i32 memoryType)
{
    transformation = Utils::FromAssimp(node->mTransformation);

    memset(name, 0, 128);
    i32 length = strlen(node->mName.C_Str());
    memcpy(name, node->mName.C_Str(), length);

    if(node->mNumChildren > 0)
    {
        childrens.Init(node->mNumChildren, memoryType);
        for(i32 i = 0; i < node->mNumChildren; ++i) {
            Node children;
            children.Init(node->mChildren[i], memoryType);
            childrens.Push(children);
        }
    }
}

void Skeleton::Init(const char* filepath, i32 memoryType)
{
    const aiScene* scene = Utils::importer.ReadFile(filepath,
        aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    ASSERT(scene);

    root.Init(scene->mRootNode, memoryType);
    
    for (i32 i = 0; i < 100; ++i) {
        finalBoneMatrices[i] = Matrix4(1.0f);
    }
    deltaTime = 0.0f;
    for (i32 i = 0; i < 10; ++i)
    {
        currentTime[i] = 0;
    }
}

void Skeleton::Animate(Animation *animation, f32 dt)
{
    deltaTime = dt;
    currentTime[0] += animation->GetTicksPerSeconds() * dt;
    currentTime[0] = fmod(currentTime[0], animation->GetDuration());
    CalculateBoneTransform(animation, &root, Matrix4(1.0f));
}

void Skeleton::CalculateBoneTransform(Animation *animation, Node *node, Matrix4 parentTransform)
{
    Matrix4 nodeTransform = node->transformation;
    //Bone *bone = node->boneIndex >= 0 ? &animation->GetBones()[node->boneIndex] : nullptr;
    HashMap<Bone>& bones = animation->GetBones();
    Bone *bone = bones.Contains(node->name) ? bones.Get(node->name) : nullptr;
    if(bone)
    {
        bone->Update(currentTime[0]);
        nodeTransform = bone->GetLocalTransform();
    }
    Matrix4 globalTransform = parentTransform * nodeTransform;

    HashMap<BoneInfo>& bonesInfo = animation->GetBonesInfo();
    if(bonesInfo.Contains(node->name))
    {
        BoneInfo *boneInfo = bonesInfo.Get(node->name);
        finalBoneMatrices[boneInfo->id] = globalTransform * boneInfo->offset;
    }

    for(i32 i = 0; i < node->childrens.size; ++i)
    {
        CalculateBoneTransform(animation, &node->childrens[i], globalTransform);
    }  
}