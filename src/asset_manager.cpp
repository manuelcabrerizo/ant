#include <asset_manager.h>
#include <memory_manager.h>

// Texture Manager
void TextureManager::Load(const char *name, const char *path)
{
     TextureHandle textureHandle;
     textureHandle.name = name;
     textureHandle.texture = GraphicsManager::Get()->TextureAlloc(path);
     nameIndex.Add(name, assets.Add(textureHandle));
}

void TextureManager::Unload(const char *name)
{
     auto handle = *nameIndex.Get(name);
     GraphicsManager::Get()->TextureFree(assets.Get(handle)->texture);
     assets.Remove(handle);
     nameIndex.Remove(name);
}

Texture *TextureManager::Get(const char *name)
{
     return AssetManager::Get(name)->texture;
}


// Model Manager
void ModelManager::Load(const char *name, const char *path)
{
     ModelHandle modelHandle;
     modelHandle.name = name;
     modelHandle.model.Init(path, STATIC_MEMORY);
     nameIndex.Add(name, assets.Add(modelHandle));
}

void ModelManager::Unload(const char *name)
{
     auto handle = *nameIndex.Get(name);
     assets.Get(handle)->model.Terminate();
     assets.Remove(handle);
     nameIndex.Remove(name);
}

Model *ModelManager::Get(const char *name)
{
     return &AssetManager::Get(name)->model;
}

// Shader Manager
void ShaderManager::Load(const char *name, const char *vertPath, const char *fragPath)
{
     Frame frame = MemoryManager::Get()->GetFrame();
     File vertFile = PlatformReadFile(vertPath, FRAME_MEMORY);
     File fragFile = PlatformReadFile(fragPath, FRAME_MEMORY);
     ShaderHandle shaderHandle;
     shaderHandle.name = name;
     shaderHandle.shader =  GraphicsManager::Get()->ShaderAlloc(vertFile, fragFile);
     MemoryManager::Get()->ReleaseFrame(frame);
     nameIndex.Add(name, assets.Add(shaderHandle));
}

void ShaderManager::Unload(const char *name)
{
     auto handle = *nameIndex.Get(name);
     GraphicsManager::Get()->ShaderFree(assets.Get(handle)->shader);
     assets.Remove(handle);
     nameIndex.Remove(name);
}

void ShaderManager::Bind(const char *name)
{
     GraphicsManager::Get()->ShaderBind(AssetManager::Get(name)->shader);
}

Shader *ShaderManager::Get(const char *name)
{
     return AssetManager::Get(name)->shader;
}


// Material Manager
void MaterialManager::Init(u32 assetsCapacity)
{
    AssetManager::Init(assetsCapacity);
    allocator.Init(STATIC_MEMORY);
}

void MaterialManager::LoadSolidColor(const char* name,
    const char* shaderName,
    ShaderManager* shaderManager,
    const Vector3& ambient,
    const Vector3& diffuse,
    const Vector3& specular,
    f32 shininess)
{
    MaterialHandle materialHandle;
    materialHandle.name = name;
    void* buffer = allocator.Alloc();
    SolidColorMaterial* material = new (buffer) SolidColorMaterial;
    material->Init(shaderName, shaderManager, ambient, diffuse, specular, shininess);
    materialHandle.material = material;
    nameIndex.Add(name, assets.Add(materialHandle));
}

void MaterialManager::LoadTexture(const char* name,
    const char* shaderName,
    ShaderManager* shaderManager,
    const char* diffuseName,
    const char* normalName,
    const char* specularName,
    f32 shininess,
    TextureManager* textureManager)
{
    MaterialHandle materialHandle;
    materialHandle.name = name;
    void* buffer = allocator.Alloc();
    TextureMaterial* material = new (buffer) TextureMaterial;
    material->Init(shaderName, shaderManager, diffuseName, normalName, specularName, shininess, textureManager);
    materialHandle.material = material;
    nameIndex.Add(name, assets.Add(materialHandle));
}


void MaterialManager::Unload(const char* name)
{
    auto handle = *nameIndex.Get(name);
    Material* material = assets.Get(handle)->material;
    material->Terminate();
    allocator.Free(material);
    assets.Remove(handle);
    nameIndex.Remove(name);
}

void MaterialManager::Bind(const char *name)
{
    AssetManager::Get(name)->material->Bind();
}

Material* MaterialManager::Get(const char* name)
{
    return AssetManager::Get(name)->material;
}

