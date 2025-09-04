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

void VertexShaderManager::Load(const char* name, const char* filePath, int memoryType)
{
    if (!Contains(name))
    {
        Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
        
        File file = PlatformReadFile(filePath, SCRATCH_MEMORY);
        VertexShaderHandle shaderHandle{};
        shaderHandle.name = name;
        shaderHandle.shader = GraphicsManager::Get()->VertexShaderAlloc(file);
        
        MemoryManager::Get()->ReleaseFrame(frame);
        
        AssetManager::Add(shaderHandle, memoryType);
    }
}

void VertexShaderManager::Unload(VertexShaderHandle* handle)
{
    GraphicsManager::Get()->VertexShaderFree(handle->shader);    
}

void VertexShaderManager::Bind(const char* name)
{
    VertexShaderHandle* handle = AssetManager::Get(name);
    GraphicsManager::Get()->VertexShaderBind(handle->shader);
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

void FragmentShaderManager::Load(const char* name, const char* filePath, int memoryType)
{
    if (!Contains(name))
    {
        Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);

        File file = PlatformReadFile(filePath, SCRATCH_MEMORY);
        FragmentShaderHandle shaderHandle{};
        shaderHandle.name = name;
        shaderHandle.shader = GraphicsManager::Get()->FragmentShaderAlloc(file);

        MemoryManager::Get()->ReleaseFrame(frame);
        
        AssetManager::Add(shaderHandle, memoryType);
    }
}

void FragmentShaderManager::Unload(FragmentShaderHandle* handle)
{
    GraphicsManager::Get()->FragmentShaderFree(handle->shader);
}

void FragmentShaderManager::Bind(const char* name)
{
    FragmentShaderHandle* handle = AssetManager::Get(name);
    GraphicsManager::Get()->FragmentShaderBind(handle->shader);
}

FragmentShader* FragmentShaderManager::Get(const char* name)
{
    return AssetManager::Get(name)->shader;
}