#pragma once

class Animation;
class Skeleton;

class  Skeleton
{
    class Node
    {
    public:
        mat4 transformation = mat4(1.0f);
        char name[128];
        Array<Node> childrens;

        //i32 FindBoneIndex(aiBone **bones, i32 count, const char *name);
        void Init(aiNode *node, i32 memoryType);
    };

private:
    mat4 finalBoneMatrices[100];
    f32 currentTime[10];
    f32 deltaTime = 0.0f;
    Node root;

    void CalculateBoneTransform(Animation *animation, Node *node, mat4 parentTransform);
public:
    void Init(const char *filepath, i32 memoryType);
    void Animate(Animation *animation, f32 dt);
    mat4 *GetMatrices()
    {
        return finalBoneMatrices;
    }
};