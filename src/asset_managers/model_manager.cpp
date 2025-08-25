#include "model_manager.h"

ModelManager ModelManager::instance;
bool ModelManager::initialize = false;

void ModelManager::Initialize(u32 assetsCapacity)
{
    if (initialize)
    {
        ASSERT(!"Error: model manager already initialize");
    }
    instance.staticAllocator.Init(STATIC_MEMORY);
    instance.frameAllocator.Init(FRAME_MEMORY);
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
    if (ShouldLoad(name))
    {
        Model* model = nullptr;
        switch (memoryType)
        {
        case STATIC_MEMORY:
        {
            model = staticAllocator.Alloc();
        } break;
        case FRAME_MEMORY:
        {
            model = frameAllocator.Alloc();
        } break;
        }

        model->Init(path, memoryType);

        void* buffer = assets.Alloc();
        ModelHandle* modelHandle = new (buffer) ModelHandle();
        modelHandle->name = name;
        modelHandle->model = model;

        AssetManager::AssetRef assetRef;
        assetRef.refCount = 1;
        assetRef.asset = modelHandle;

        nameIndex.Add(name, assetRef);
    }
}

void ModelManager::Unload(const char* name)
{
    if (ShouldUnload(name))
    {
        ModelHandle* modelHandle = nameIndex.Get(name)->asset;
        Model* model = modelHandle->model;
        model->Terminate();

        switch (model->GetMemoryType())
        {
        case STATIC_MEMORY:
        {
            model = staticAllocator.Alloc();
        } break;
        case FRAME_MEMORY:
        {
            model = frameAllocator.Alloc();
        } break;
        }

        nameIndex.Remove(name);
        assets.Free(modelHandle);
    }
}

Model* ModelManager::Get(const char* name)
{
    return AssetManager::Get(name)->model;
}