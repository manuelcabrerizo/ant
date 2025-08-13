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
    // using a block allocator for polymorphism
    // the size of the block should always be at least the size
    // of the larger material derived class
    static_assert(KB(2) >= sizeof(SolidColorMaterial), "not enough memory for SolidColorMateial");
    static_assert(KB(2) >= sizeof(TextureMaterial), "not enough memory for TextureMaterial");
    BlockAllocator<KB(2), 100> allocator;
public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static MaterialManager* Get();

    void Init(u32 assetsCapacity) override;

    void LoadSolidColor(const char* name,
        const char* shaderName,
        const Vector3& ambient,
        const Vector3& diffuse,
        const Vector3& specular,
        f32 shininess);

    void LoadTexture(const char* name,
        const char* shaderName,
        const char* diffuseName,
        const char* normalName,
        const char* specularName,
        f32 shininess);

    void Unload(const char* name) override;
    void Bind(const char* name);
    Material* Get(const char* name);
};