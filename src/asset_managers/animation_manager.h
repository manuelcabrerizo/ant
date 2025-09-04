#pragma once

#include "asset_manager.h"
#include <animation/skeleton.h>
#include <animation/animation.h>

class Model;

struct SkeletonHandle
{
    const char* name;
    Skeleton* skeleton;
};

class SkeletonManager : public AssetManager<SkeletonHandle>
{
private:
    static SkeletonManager instance;
    static bool initialize;
    SkeletonManager() {}

    void Unload(SkeletonHandle* handle) override;
public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static SkeletonManager* Get();

    void Load(const char* name, const char* path, int memoryType);
    Skeleton* Get(const char* name);
};

struct AnimationHandle
{
    const char* name;
    Animation* animation;
};

class AnimationManager : public AssetManager<AnimationHandle>
{
private:
    static AnimationManager instance;
    static bool initialize;
    AnimationManager() {}

    void Unload(AnimationHandle* handle) override;
public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static AnimationManager* Get();

    void Load(const char* name, const char* path, Model* model, int memoryType);
    Animation* Get(const char* name);
};