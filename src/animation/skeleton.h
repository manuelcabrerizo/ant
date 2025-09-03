#pragma once

#include <containers.h>
#include <math/matrix4.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Animation;

class  Skeleton
{
    class Node
    {
    public:
        Matrix4 transformation = Matrix4(1.0f);
        char name[128] = {};
        Array<Node> childrens;
        void Init(aiNode *node, i32 memoryType);
    };

private:
    Node root;
    int memoryType;
public:
    void Init(const char *filepath, i32 memoryType);

    void CalculateBoneTransform(Animation* animation, Node* node, Matrix4 parentTransform,
        Matrix4 finalBoneMatrices[], float currentTime);

    Node* GetRoot()
    {
        return &root;
    }

    int GetMemoryType()
    {
        return memoryType;
    }
};