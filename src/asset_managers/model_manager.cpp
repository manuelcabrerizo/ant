#include "model_manager.h"

ModelManager ModelManager::instance;
bool ModelManager::initialize = false;

void ModelManager::Initialize(u32 assetsCapacity)
{
    if (initialize)
    {
        ASSERT(!"Error: model manager already initialize");
    }
    instance.allocator.Init(STATIC_MEMORY);
    instance.Init(assetsCapacity);
    initialize = true;
}

void ModelManager::Shutdown()
{
    if (!initialize)
    {
        ASSERT(!"Error: model manager has not been initialize");
    }
    instance.Terminate();
    initialize = false;
}

ModelManager* ModelManager::Get()
{
    if (!initialize)
    {
        ASSERT(!"Error: model manager has not been initialize");
    }
    return &instance;
}

void ModelManager::Load(const char* name, const char* path)
{
    if (!nameIndex.Contains(name))
    {
        Model* model = allocator.Alloc();
        model->Init(path, STATIC_MEMORY);

        ModelHandle modelHandle;
        modelHandle.name = name;
        modelHandle.model = model;
        nameIndex.Add(name, assets.Add(modelHandle));
    }
}

void ModelManager::Unload(const char* name)
{
    auto handle = *nameIndex.Get(name);
    assets.Get(handle)->model->Terminate();
    assets.Remove(handle);
    nameIndex.Remove(name);
}

Model* ModelManager::Get(const char* name)
{
    return AssetManager::Get(name)->model;
}