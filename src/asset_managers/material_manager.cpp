#include "material_manager.h"

MaterialManager MaterialManager::instance;
bool MaterialManager::initialize = false;

void MaterialManager::Initialize(u32 assetsCapacity)
{
    if (initialize)
    {
        ASSERT(!"Error: material manager already initialize");
    }
    instance.Init(assetsCapacity);
    initialize = true;
}

void MaterialManager::Shutdown()
{
    if (!initialize)
    {
        ASSERT(!"Error: material manager has not been initialize");
    }
    instance.Terminate();
    initialize = false;
}

MaterialManager* MaterialManager::Get()
{
    if (!initialize)
    {
        ASSERT(!"Error: material manager has not been initialize");
    }
    return &instance;
}

void MaterialManager::Init(u32 assetsCapacity)
{
    AssetManager::Init(assetsCapacity);
    allocator.Init(100, STATIC_MEMORY);
}

void MaterialManager::LoadSolidColor(const char* name,
    const char* shaderName,
    const Vector3& ambient,
    const Vector3& diffuse,
    const Vector3& specular,
    f32 shininess)
{
    MaterialHandle materialHandle = {};
    memcpy((void*)materialHandle.name, (void*)name, strlen(name));
    void* buffer = allocator.Alloc();
    SolidColorMaterial* material = new (buffer) SolidColorMaterial;
    material->Init(shaderName, ambient, diffuse, specular, shininess);
    materialHandle.material = material;
    nameIndex.Add(name, assets.Add(materialHandle));
}

void MaterialManager::LoadTexture(const char* name,
    const char* shaderName,
    const char* diffuseName,
    const char* normalName,
    const char* specularName,
    f32 shininess)
{
    MaterialHandle materialHandle = {};
    memcpy((void*)materialHandle.name, (void*)name, strlen(name));
    void* buffer = allocator.Alloc();
    TextureMaterial* material = new (buffer) TextureMaterial;
    material->Init(shaderName, diffuseName, normalName, specularName, shininess);
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

void MaterialManager::Bind(const char* name)
{
    AssetManager::Get(name)->material->Bind();
}

Material* MaterialManager::Get(const char* name)
{
    return AssetManager::Get(name)->material;
}