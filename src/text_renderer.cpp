#include "text_renderer.h"
#include "batch_renderer.h"
#include "asset_managers/texture_manager.h"
#include <utils.h>
#include <strings.h>
#include <asset_managers/shader_manager.h>
#include <math/algebra.h>

void TextRenderer::Init(const char* atlasInfoPath, int memoryType)
{
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    
    File fencesFile = PlatformReadFile(atlasInfoPath, SCRATCH_MEMORY);
    FileReader reader = FileReader(&fencesFile);
    while (const char* line = reader.GetNextLine())
    {
        switch (line[0])
        {
        case 'a':
        {
            memset(info.imagePath, 0, 256);
            if (sscanf(line, "a %s %d %d %d %d %d", info.imagePath, &info.width, &info.height, &info.spriteCount, &info.isFont, &info.fontSize) == 6)
            {
                sprites.Init(info.spriteCount, memoryType);
            }
        } break;
        case 's': 
        {
            FontSpriteInfo spriteInfo;
            if (sscanf(line, "s %s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                spriteInfo.nameId, spriteInfo.tag,
                &spriteInfo.originX, &spriteInfo.originY, 
                &spriteInfo.positionX, &spriteInfo.positionY,
                &spriteInfo.sourceWidth, &spriteInfo.sourceHeight,
                &spriteInfo.padding,
                &spriteInfo.trimmed,
                &spriteInfo.trimRecX, &spriteInfo.trimRecY, &spriteInfo.trimRecWidth, &spriteInfo.trimRecHeight,
                &spriteInfo.colliderType,
                &spriteInfo.colliderPosX, &spriteInfo.colliderPosY, &spriteInfo.colliderSizeX, &spriteInfo.colliderSizeY,
                &spriteInfo.charValue,
                &spriteInfo.charOffsetX, &spriteInfo.charOffsetY,
                &spriteInfo.charAdvanceX) == 23)
            {
                sprites.Push(spriteInfo);
            }
        } break;
        }
    }

    // Get path
    char* texturePath = (char*)MemoryManager::Get()->Alloc(_MAX_PATH, SCRATCH_MEMORY);
    memset(texturePath, 0, _MAX_PATH);
    i32 prevIndex = Strings::FindLastInstance(atlasInfoPath, '/');
    memcpy(texturePath, atlasInfoPath, prevIndex + 1);
    memcpy(texturePath + strlen(texturePath), info.imagePath, sizeof(info.imagePath));

    TextureManager::Get()->Load(info.imagePath, texturePath, memoryType);
    atlas = TextureManager::Get()->Get(info.imagePath);

    MemoryManager::Get()->ReleaseFrame(frame);

    // Init the batch renderer
    batch = GraphicsManager::Get()->BatchRendererAlloc(
        VertexShaderManager::Get()->Get("batch_vert"),
        FragmentShaderManager::Get()->Get("batch_frag"),
        atlas);


    int width, height;
    PlatformClientDimensions(&width, &height);
    matrices.view = Matrix4();
    matrices.proj = Matrix4::Ortho(0, width, 0, height, 0, 100);
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &matrices, sizeof(TextUbo), 0);
}

void TextRenderer::Terminate()
{
    GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
    GraphicsManager::Get()->BatchRendererFree(batch);
}

void TextRenderer::OnResize(const Vector2& extent)
{
    matrices.proj = Matrix4::Ortho(0, extent.x, 0, extent.y, 0, 100);
}

void TextRenderer::DrawString(const char* text, const Vector2& position, float size)
{
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &matrices);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);

    // TODO: cache the uvs once we know how to generate them correctly
    float cursorX = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        char letter = text[i];

        if (letter == 'i')
        {
            int StopHEre = 0;
        }

        ASSERT(letter >= 32);
        auto& sprite = sprites[letter - 32];

        float startX = sprite.positionX;
        float startY = sprite.positionY;
        float endX =  sprite.positionX + sprite.sourceWidth;
        float endY = sprite.positionY + sprite.sourceHeight;
        startX /= static_cast<float>(info.width);
        startY /= static_cast<float>(info.height);
        endX /= static_cast<float>(info.width);
        endY /= static_cast<float>(info.height);

        float baseY = (sprite.sourceHeight / 2);
        float baseX = (sprite.sourceWidth / 2);

        float offsetY = (info.fontSize - sprite.sourceHeight) - sprite.charOffsetY;
        float offsetX = sprite.charOffsetX;

        batch->DrawQuad(Vector3(
            baseX + position.x + cursorX + offsetX,
            baseY + position.y + offsetY, 0),
            Vector3(sprite.sourceWidth, sprite.sourceHeight, 1), Matrix4(),
            Vector4(endX, 1-endY, startX, 1-startY), Vector3(4, 4, 0));

        cursorX += static_cast<float>(sprite.charAdvanceX);
    }
    batch->Present();
}
