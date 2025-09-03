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

    ObjectAllocator<Skeleton> staticAllocator;
    ObjectAllocator<Skeleton> frameAllocator;

public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static void ResetFrameAllocator();
    static SkeletonManager* Get();

    void Load(const char* name, const char* path, int memoryType);
    void Unload(const char* name) override;
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

    ObjectAllocator<Animation> staticAllocator;
    ObjectAllocator<Animation> frameAllocator;

public:
    static void Initialize(u32 assetsCapacity);
    static void Shutdown();
    static void ResetFrameAllocator();
    static AnimationManager* Get();

    void Load(const char* name, const char* path, Model* model, int memoryType);
    void Unload(const char* name) override;
    Animation* Get(const char* name);
};