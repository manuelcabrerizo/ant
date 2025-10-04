#include "texture_manager.h"

TextureManager TextureManager::instance;
bool TextureManager::initialize = false;

void TextureManager::Initialize(u32 assetsCapacity)
{
    if (initialize)
    {
        ASSERT(!"Error: texture manager already initialize");
    }
    instance.Init(assetsCapacity);
    initialize = true;
}

void TextureManager::Shutdown()
{
    if (!initialize)
    {
        ASSERT(!"Error: texture manager has not been initialize");
    }
    instance.Terminate();
    initialize = false;
}

TextureManager* TextureManager::Get()
{
    if (!initialize)
    {
        ASSERT(!"Error: texture manager has not been initialize");
    }
    return &instance;
}

void TextureManager::Load(const char* name, const char* path, int memoryType, bool isSrgb)
{
    if (!Contains(name))
    {
        TextureHandle textureHandle{};
        memcpy((void*)textureHandle.name, (void*)name, strlen(name));
        textureHandle.texture = GraphicsManager::Get()->TextureAlloc(path, isSrgb);

        AssetManager::Add(textureHandle, memoryType);
    }
}

void TextureManager::Unload(TextureHandle* handle)
{
    GraphicsManager::Get()->TextureFree(handle->texture);   
}

Texture* TextureManager::Get(const char* name)
{
    return AssetManager::Get(name)->texture;
}
