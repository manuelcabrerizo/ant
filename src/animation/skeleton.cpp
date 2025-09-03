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
            if (childrens.capacity == 0)
            {
                int StopHere = 0;
            }
            childrens.Push(children);
        }
    }
}

void Skeleton::Init(const char* filepath, i32 memoryType)
{
    this->memoryType = memoryType;
    const aiScene* scene = Utils::importer.ReadFile(filepath,
        aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    ASSERT(scene);

    root.Init(scene->mRootNode, memoryType);
}

void Skeleton::CalculateBoneTransform(Animation *animation, Node *node, Matrix4 parentTransform,
    Matrix4 finalBoneMatrices[], float currentTime)
{
    Matrix4 nodeTransform = node->transformation;
    //Bone *bone = node->boneIndex >= 0 ? &animation->GetBones()[node->boneIndex] : nullptr;
    HashMap<Bone>& bones = animation->GetBones();
    Bone *bone = bones.Contains(node->name) ? bones.Get(node->name) : nullptr;
    if(bone)
    {
        nodeTransform = bone->Update(currentTime);
    }
    Matrix4 globalTransform = nodeTransform * parentTransform;

    HashMap<BoneInfo>& bonesInfo = animation->GetBonesInfo();
    if(bonesInfo.Contains(node->name))
    {
        BoneInfo *boneInfo = bonesInfo.Get(node->name);
        finalBoneMatrices[boneInfo->id] = boneInfo->offset * globalTransform;
    }

    for(i32 i = 0; i < node->childrens.size; ++i)
    {
        CalculateBoneTransform(animation, &node->childrens[i], globalTransform,
            finalBoneMatrices, currentTime);
    }  
}