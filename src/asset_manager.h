#pragma once

#include <containers.h>
#include <model.h>
#include <graphics_manager.h>
#include <material.h>

template <typename Type>
class AssetManager
{
protected:
     Slotmap<Type> assets;
     HashMap<SlotmapKey<Type>> nameIndex;
public:
     virtual ~AssetManager() {}
     
     virtual void Init(u32 assetsCapacity);
     void Terminate();
     void Clear();
     bool Contains(const char* name);

     virtual void Unload(const char *name) = 0;
protected:
     Type *Get(const char *name);
     Type *Get(SlotmapKey<Type> handle);
     SlotmapKey<Type> GetHandle(const char *name);
};

template <typename Type>
void AssetManager<Type>::Init(u32 assetsCapacity)
{
    nameIndex.Init(assetsCapacity, STATIC_MEMORY);
    assets.Init(assetsCapacity, STATIC_MEMORY);
}

template <typename Type>
void AssetManager<Type>::Terminate()
{
    Clear();
}

template <typename Type>
void AssetManager<Type>::Clear()
{
    for (i32 i = assets.GetArray()->size - 1; i >= 0; --i)
    {
        Type* asset = &assets.GetArray()->data[i];
        Unload(asset->name);
    }
}

template <typename Type>
Type* AssetManager<Type>::Get(const char* name)
{
    return assets.Get(*nameIndex.Get(name));
}

template <typename Type>
Type* AssetManager<Type>::Get(SlotmapKey<Type> handle)
{
    return assets.Get(handle);
}

template <typename Type>
SlotmapKey<Type> AssetManager<Type>::GetHandle(const char* name)
{
    return *nameIndex.Get(name);
}

template <typename Type>
bool AssetManager<Type>::Contains(const char* name)
{
    return nameIndex.Contains(name);
}

// Texture Manager
struct TextureHandle
{
     const  char *name;
     Texture *texture;
};

class TextureManager : public AssetManager<TextureHandle>
{
public:
     void Load(const char *name, const char *path);
     void Unload(const char *name) override;
     Texture *Get(const char *name);
};

// Model Manager
struct ModelHandle
{
     const char *name;
     Model model;
};

class ModelManager : public AssetManager<ModelHandle>
{
public:
     void Load(const char *name, const char *path);
     void Unload(const char *name) override;
     Model *Get(const char *name);
};

// Shader Manager
struct ShaderHandle
{
     const char *name;
     Shader *shader;
};

class ShaderManager : public AssetManager<ShaderHandle>
{
public:
     void Load(const char *name, const char *vertPath, const char *fragPath);
     void Unload(const char *name) override;
     void Bind(const char *name);
     Shader *Get(const char *name);
};


// Material Manager
struct MaterialHandle
{
    const char *name;
    Material *material;
};

class MaterialManager : public AssetManager<MaterialHandle>
{
private:
    // using a block allocator for polymorphism
    // the size of the block should always be at least the size
    // of the larger material derived class
    static_assert(KB(2) >= sizeof(SolidColorMaterial), "not enough memory for SolidColorMateial");
    static_assert(KB(2) >= sizeof(TextureMaterial), "not enough memory for TextureMaterial");
    BlockAllocator<KB(2)> allocator;
public:
    void Init(u32 assetsCapacity) override;

    void LoadSolidColor(const char* name,
        const char* shaderName,
        ShaderManager* shaderManager,
        const Vector3& ambient,
        const Vector3& diffuse,
        const Vector3& specular,
        f32 shininess);
    
    void LoadTexture(const char* name,
        const char* shaderName,
        ShaderManager* shaderManager,
        const char* diffuseName,
        const char* normalName,
        const char* specularName,
        f32 shininess,
        TextureManager* textureManager);

    void Unload(const char *name) override;
    void Bind(const char *name);
    Material* Get(const char *name);
};
