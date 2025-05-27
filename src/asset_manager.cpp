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
void VertexShaderManager::Load(const char *name, const char *filePath)
{
     Frame frame = MemoryManager::Get()->GetFrame();
     File file = PlatformReadFile(filePath, FRAME_MEMORY);
     VertexShaderHandle shaderHandle;
     shaderHandle.name = name;
     shaderHandle.shader =  GraphicsManager::Get()->VertexShaderAlloc(file);
     MemoryManager::Get()->ReleaseFrame(frame);
     nameIndex.Add(name, assets.Add(shaderHandle));
}

void VertexShaderManager::Unload(const char *name)
{
     auto handle = *nameIndex.Get(name);
     GraphicsManager::Get()->VertexShaderFree(assets.Get(handle)->shader);
     assets.Remove(handle);
     nameIndex.Remove(name);
}

void VertexShaderManager::Bind(const char *name)
{
     GraphicsManager::Get()->VertexShaderBind(AssetManager::Get(name)->shader);
}

VertexShader* VertexShaderManager::Get(const char *name)
{
     return AssetManager::Get(name)->shader;
}

// Fragment Shader
void FragmentShaderManager::Load(const char* name, const char* filePath)
{
    Frame frame = MemoryManager::Get()->GetFrame();
    File file = PlatformReadFile(filePath, FRAME_MEMORY);
    FragmentShaderHandle shaderHandle;
    shaderHandle.name = name;
    shaderHandle.shader = GraphicsManager::Get()->FragmentShaderAlloc(file);
    MemoryManager::Get()->ReleaseFrame(frame);
    nameIndex.Add(name, assets.Add(shaderHandle));
}

void FragmentShaderManager::Unload(const char* name)
{
    auto handle = *nameIndex.Get(name);
    GraphicsManager::Get()->FragmentShaderFree(assets.Get(handle)->shader);
    assets.Remove(handle);
    nameIndex.Remove(name);
}

void FragmentShaderManager::Bind(const char* name)
{
    GraphicsManager::Get()->FragmentShaderBind(AssetManager::Get(name)->shader);
}

FragmentShader* FragmentShaderManager::Get(const char* name)
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
    FragmentShaderManager* shaderManager,
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
    FragmentShaderManager* shaderManager,
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

