#include "model_manager.h"
#include <memory_manager.h>

ModelManager ModelManager::instance;
bool ModelManager::initialize = false;

void ModelManager::Initialize(u32 assetsCapacity)
{
    if (initialize)
    {
        ASSERT(!"Error: model manager already initialize");
    }
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


void ModelManager::Load(const char* name, const char* path, int memoryType)
{
    if (!Contains(name))
    {
        void* buffer = MemoryManager::Get()->Alloc(sizeof(Model), memoryType);
        Model* model = new (buffer) Model();
        model->Init(path, memoryType);

        ModelHandle modelHandle{};
        modelHandle.name = name;
        modelHandle.model = model;

        AssetManager::Add(modelHandle, memoryType);
    }
}

void ModelManager::Unload(ModelHandle* handle)
{
    handle->model->Terminate();
}

Model* ModelManager::Get(const char* name)
{
    return AssetManager::Get(name)->model;
}