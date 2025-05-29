#pragma once

#include "asset_manager.h"
#include <graphics_manager.h>

struct TextureHandle
{
    char name[_MAX_PATH + 1];
    Texture* texture;
};

class TextureManager : public AssetManager<TextureHandle>
{
private:
    static TextureManager instance;
    static bool initialize;
    TextureManager() {}
public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static TextureManager* Get();

    void Load(const char* name, const char* path);
    void Unload(const char* name) override;
    Texture* Get(const char* name);
};