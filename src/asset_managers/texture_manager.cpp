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

void TextureManager::Load(const char* name, const char* path)
{
    if (!nameIndex.Contains(name))
    {
        TextureHandle textureHandle = {};
        memcpy((void*)textureHandle.name, (void*)name, strlen(name));
        textureHandle.texture = GraphicsManager::Get()->TextureAlloc(path);
        nameIndex.Add(textureHandle.name, assets.Add(textureHandle));
    }
}

void TextureManager::Unload(const char* name)
{
    auto handle = *nameIndex.Get(name);
    GraphicsManager::Get()->TextureFree(assets.Get(handle)->texture);
    assets.Remove(handle);
    nameIndex.Remove(name);
}

Texture* TextureManager::Get(const char* name)
{
    return AssetManager::Get(name)->texture;
}