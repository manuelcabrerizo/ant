#pragma once

#include <graphics_manager.h>
#include <math/vector2.h>
#include <math/vector4.h>
#include <containers.h>

struct TextUbo
{
    Matrix4 view;
    Matrix4 proj;
        
};

class BatchRenderer;
class Texture;

struct FontInfo
{
    int width, height, spriteCount, isFont, fontSize;
    char imagePath[256];
};

struct FontSpriteInfo
{
    char nameId[256];
    char tag[256];
    int originX, originY;
    int positionX, positionY;
    int sourceWidth, sourceHeight;
    int padding;
    bool trimmed;
    int trimRecX, trimRecY, trimRecWidth, trimRecHeight;
    int colliderType;
    int colliderPosX, colliderPosY, colliderSizeX, colliderSizeY;
    // Atlas contains font data
    int charValue;
    int charOffsetX, charOffsetY;
    int charAdvanceX;
};

class TextRenderer
{
private:
    UniformBuffer* uniformBuffer = nullptr;
    TextUbo matrices;

    BatchRenderer *batch = nullptr;
    Texture* atlas = nullptr;

    Array<FontSpriteInfo> sprites;
    FontInfo info;

public:
    void Init(const char* atlasInfoPath, int memoryType);
    void Terminate();
    void OnResize(const Vector2& extent);

    void DrawString(const char* text, const Vector2& position, float size);
};