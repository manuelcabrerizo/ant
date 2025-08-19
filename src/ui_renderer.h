#pragma once

#include <graphics_manager.h>
#include <math/vector2.h>
#include <math/matrix4.h>

struct UIVertex
{
    Vector2 pos;
    Vector2 uvs;
};

struct UIUbo
{
    Matrix4 view;
    Matrix4 proj;
    // Use batch rendering intead of the model matrix
    Matrix4 model;
};

class UIRenderer
{
private:
    VertexBuffer* quad = nullptr;
    UniformBuffer* uniformBuffer = nullptr;
    UIUbo matrices;

public:
    void Init();
    void Terminate();
    void DrawQuat(const Vector2& position, const Vector2& size, int zIndex, const char* textureName);
};

