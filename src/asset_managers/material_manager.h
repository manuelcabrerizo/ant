#pragma once

#include "asset_manager.h"
#include <material.h>

struct MaterialHandle
{
    char name[_MAX_PATH];
    Material* material;
};

class MaterialManager : public AssetManager<MaterialHandle>
{
private:
    static MaterialManager instance;
    static bool initialize;
    MaterialManager() {}

    void Unload(MaterialHandle* handle) override;

public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static MaterialManager* Get();

    void LoadSolidColor(const char* name,
        FragmentShader* shader,
        const Vector3& ambient,
        const Vector3& diffuse,
        const Vector3& specular,
        f32 shininess, int memoryType);

    void LoadTexture(const char* name,
        FragmentShader* shader,
        Texture* diffuse,
        Texture* normal,
        Texture* specular,
        f32 shininess, int memoryType);

    void Bind(const char* name);
    Material* Get(const char* name);
};