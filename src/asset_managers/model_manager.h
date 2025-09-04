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

    void Unload(ModelHandle* handle) override;

public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static ModelManager* Get();

    void Load(const char* name, const char* path, int memoryType);
    Model* Get(const char* name);
};