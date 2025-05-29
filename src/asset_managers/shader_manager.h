#pragma once

#include "asset_manager.h"
#include <graphics_manager.h>

struct VertexShaderHandle
{
    const char* name;
    VertexShader* shader;
};

class VertexShaderManager : public AssetManager<VertexShaderHandle>
{
private:
    static VertexShaderManager instance;
    static bool initialize;
    VertexShaderManager() {}
public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static VertexShaderManager* Get();

    void Load(const char* name, const char* filePath);
    void Unload(const char* name) override;
    void Bind(const char* name);
    VertexShader* Get(const char* name);
};

struct FragmentShaderHandle
{
    const char* name;
    FragmentShader* shader;
};

class FragmentShaderManager : public AssetManager<FragmentShaderHandle>
{
private:
    static FragmentShaderManager instance;
    static bool initialize;
    FragmentShaderManager() {}
public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static FragmentShaderManager* Get();

    void Load(const char* name, const char* filePath);
    void Unload(const char* name) override;
    void Bind(const char* name);
    FragmentShader* Get(const char* name);
};