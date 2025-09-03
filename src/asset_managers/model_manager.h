#pragma once


#include "asset_manager.h"
#include <model.h>

struct ModelHandle
{
    const char* name;
    Model* model;
};

class ModelManager : public AssetManager<ModelHandle>
{
private:
    static ModelManager instance;
    static bool initialize;
    ModelManager() {}

    ObjectAllocator<Model> staticAllocator;
    ObjectAllocator<Model> frameAllocator;

public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static ModelManager* Get();
    static void ResetFrameAllocator();

    void Load(const char* name, const char* path, int memoryType);
    void Unload(const char* name) override;
    Model* Get(const char* name);
};