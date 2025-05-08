#pragma once

class Animation;
class Skeleton;

class  Skeleton
{
    class Node
    {
    public:
        Matrix4 transformation = Matrix4(1.0f);
        char name[128];
        Array<Node> childrens;

        //i32 FindBoneIndex(aiBone **bones, i32 count, const char *name);
        void Init(aiNode *node, i32 memoryType);
    };

private:
    Matrix4 finalBoneMatrices[100];
    f32 currentTime[10];
    f32 deltaTime = 0.0f;
    Node root;

    void CalculateBoneTransform(Animation *animation, Node *node, Matrix4 parentTransform);
public:
    void Init(const char *filepath, i32 memoryType);
    void Animate(Animation *animation, f32 dt);
    Matrix4 *GetMatrices()
    {
        return finalBoneMatrices;
    }
};