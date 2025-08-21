#include "shader_manager.h"

VertexShaderManager VertexShaderManager::instance;
bool VertexShaderManager::initialize = false;

void VertexShaderManager::Initialize(u32 assetsCapacity)
{
    if (initialize)
    {
        ASSERT(!"Error: model manager already initialize");
    }
    instance.Init(assetsCapacity);
    initialize = true;
}

void VertexShaderManager::Shutdown()
{
    if (!initialize)
    {
        ASSERT(!"Error: model manager has not been initialize");
    }
    instance.Terminate();
    initialize = false;
}

VertexShaderManager* VertexShaderManager::Get()
{
    if (!initialize)
    {
        ASSERT(!"Error: model manager has not been initialize");
    }
    return &instance;
}

void VertexShaderManager::Load(const char* name, const char* filePath)
{
    if (!nameIndex.Contains(name))
    {
        Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
        File file = PlatformReadFile(filePath, SCRATCH_MEMORY);
        VertexShaderHandle shaderHandle;
        shaderHandle.name = name;
        shaderHandle.shader = GraphicsManager::Get()->VertexShaderAlloc(file);
        MemoryManager::Get()->ReleaseFrame(frame);
        nameIndex.Add(name, assets.Add(shaderHandle));
    }
}

void VertexShaderManager::Unload(const char* name)
{
    auto handle = *nameIndex.Get(name);
    GraphicsManager::Get()->VertexShaderFree(assets.Get(handle)->shader);
    assets.Remove(handle);
    nameIndex.Remove(name);
}

void VertexShaderManager::Bind(const char* name)
{
    GraphicsManager::Get()->VertexShaderBind(AssetManager::Get(name)->shader);
}

VertexShader* VertexShaderManager::Get(const char* name)
{
    return AssetManager::Get(name)->shader;
}

FragmentShaderManager FragmentShaderManager::instance;
bool FragmentShaderManager::initialize = false;

void FragmentShaderManager::Initialize(u32 assetsCapacity)
{
    if (initialize)
    {
        ASSERT(!"Error: model manager already initialize");
    }
    instance.Init(assetsCapacity);
    initialize = true;
}

void FragmentShaderManager::Shutdown()
{
    if (!initialize)
    {
        ASSERT(!"Error: model manager has not been initialize");
    }
    instance.Terminate();
    initialize = false;
}

FragmentShaderManager* FragmentShaderManager::Get()
{
    if (!initialize)
    {
        ASSERT(!"Error: model manager has not been initialize");
    }
    return &instance;
}

void FragmentShaderManager::Load(const char* name, const char* filePath)
{
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File file = PlatformReadFile(filePath, SCRATCH_MEMORY);
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