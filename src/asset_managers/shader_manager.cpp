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
    if (ShouldLoad(name))
    {
        Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
        File file = PlatformReadFile(filePath, SCRATCH_MEMORY);
        void* buffer = assets.Alloc();
        VertexShaderHandle* shaderHandle = new (buffer) VertexShaderHandle();
        shaderHandle->name = name;
        shaderHandle->shader = GraphicsManager::Get()->VertexShaderAlloc(file);
        MemoryManager::Get()->ReleaseFrame(frame);
        AssetManager::AssetRef assetRef;
        assetRef.refCount = 1;
        assetRef.asset = shaderHandle;
        nameIndex.Add(name, assetRef);
    }
}

void VertexShaderManager::Unload(const char* name)
{
    if (ShouldUnload(name))
    {
        VertexShaderHandle* shaderHandle = nameIndex.Get(name)->asset;
        GraphicsManager::Get()->VertexShaderFree(shaderHandle->shader);
        nameIndex.Remove(name);
        assets.Free(shaderHandle);
    }
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
    if (ShouldLoad(name))
    {
        Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
        File file = PlatformReadFile(filePath, SCRATCH_MEMORY);
        void* buffer = assets.Alloc();
        FragmentShaderHandle* shaderHandle = new (buffer) FragmentShaderHandle();
        shaderHandle->name = name;
        shaderHandle->shader = GraphicsManager::Get()->FragmentShaderAlloc(file);
        MemoryManager::Get()->ReleaseFrame(frame);
        AssetManager::AssetRef assetRef;
        assetRef.refCount = 1;
        assetRef.asset = shaderHandle;
        nameIndex.Add(name, assetRef);
    }
}

void FragmentShaderManager::Unload(const char* name)
{
    if (ShouldUnload(name))
    {
        FragmentShaderHandle* shaderHandle = nameIndex.Get(name)->asset;
        GraphicsManager::Get()->FragmentShaderFree(shaderHandle->shader);
        nameIndex.Remove(name);
        assets.Free(shaderHandle);
    }
}

void FragmentShaderManager::Bind(const char* name)
{
    GraphicsManager::Get()->FragmentShaderBind(AssetManager::Get(name)->shader);
}

FragmentShader* FragmentShaderManager::Get(const char* name)
{
    return AssetManager::Get(name)->shader;
}