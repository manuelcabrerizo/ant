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

void MaterialManager::LoadSolidColor(const char* name,
    FragmentShader* shader,
    const Vector3& ambient,
    const Vector3& diffuse,
    const Vector3& specular,
    f32 shininess, int memoryType)
{
    if (!Contains(name))
    {
        void* buffer = MemoryManager::Get()->Alloc(sizeof(SolidColorMaterial), memoryType);
        SolidColorMaterial* material = new (buffer) SolidColorMaterial;
        material->Init(shader, ambient, diffuse, specular, shininess);

        MaterialHandle materialHandle{};
        memcpy((void*)materialHandle.name, (void*)name, strlen(name));
        materialHandle.material = material;

        AssetManager::Add(materialHandle, memoryType);
    }
}

void MaterialManager::LoadTexture(const char* name,
    FragmentShader* shader,
    Texture* diffuse,
    Texture* normal,
    Texture* specular,
    f32 shininess, int memoryType)
{
    if (!Contains(name))
    {
        void* buffer = MemoryManager::Get()->Alloc(sizeof(TextureMaterial), memoryType);
        TextureMaterial* material = new (buffer) TextureMaterial;
        material->Init(shader, diffuse, normal, specular, shininess);

        MaterialHandle materialHandle{};
        memcpy((void*)materialHandle.name, (void*)name, strlen(name));
        materialHandle.material = material;

        AssetManager::Add(materialHandle, memoryType);
    }
}


void MaterialManager::Unload(MaterialHandle* handle)
{
    handle->material->Terminate();
}

void MaterialManager::Bind(const char* name)
{
    MaterialHandle* handle = AssetManager::Get(name);
    handle->material->Bind();
}

Material* MaterialManager::Get(const char* name)
{
    return AssetManager::Get(name)->material;
}