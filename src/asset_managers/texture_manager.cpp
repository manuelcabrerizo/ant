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
    if (ShouldLoad(name))
    {
        void *buffer = assets.Alloc();
        TextureHandle* textureHandle = new (buffer) TextureHandle();
        memcpy((void*)textureHandle->name, (void*)name, strlen(name));
        textureHandle->texture = GraphicsManager::Get()->TextureAlloc(path);
        AssetManager::AssetRef assetRef;
        assetRef.refCount = 1;
        assetRef.asset = textureHandle;
        nameIndex.Add(textureHandle->name, assetRef);
    }
}

void TextureManager::Unload(const char* name)
{
    if (ShouldUnload(name))
    {
        TextureHandle* textureHandle = nameIndex.Get(name)->asset;
        GraphicsManager::Get()->TextureFree(textureHandle->texture);
        nameIndex.Remove(name);
        assets.Free(textureHandle);
    }
}

Texture* TextureManager::Get(const char* name)
{
    return AssetManager::Get(name)->texture;
}