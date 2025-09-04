#include "animation_manager.h"

// SkeletonManager Implementation

SkeletonManager SkeletonManager::instance;
bool SkeletonManager::initialize = false;

void SkeletonManager::Initialize(u32 assetsCapacity)
{
    if (initialize)
    {
        ASSERT(!"Error: skeleton manager already initialize");
    }
    instance.Init(assetsCapacity);
    initialize = true;
}

void SkeletonManager::Shutdown()
{
    if (!initialize)
    {
        ASSERT(!"Error: skeleton manager has not been initialize");
    }
    instance.Terminate();
    initialize = false;
}

SkeletonManager* SkeletonManager::Get()
{
    if (!initialize)
    {
        ASSERT(!"Error: skeleton manager has not been initialize");
    }
    return &instance;
}

void SkeletonManager::Load(const char* name, const char* path, int memoryType)
{
    if (!Contains(name))
    {
        void* buffer = MemoryManager::Get()->Alloc(sizeof(Skeleton), memoryType);
        Skeleton* skeleton = new (buffer) Skeleton();
        skeleton->Init(path, memoryType);

        SkeletonHandle skeletonHandle{};
        skeletonHandle.name = name;
        skeletonHandle.skeleton = skeleton;

        AssetManager::Add(skeletonHandle, memoryType);
    }
}

void SkeletonManager::Unload(SkeletonHandle* handle)
{
}

Skeleton* SkeletonManager::Get(const char* name)
{
    return AssetManager::Get(name)->skeleton;
}

// AnimationManager Implementation

AnimationManager AnimationManager::instance;
bool AnimationManager::initialize = false;

void AnimationManager::Initialize(u32 assetsCapacity)
{
    if (initialize)
    {
        ASSERT(!"Error: animation manager already initialize");
    }
    instance.Init(assetsCapacity);
    initialize = true;
}

void AnimationManager::Shutdown()
{
    if (!initialize)
    {
        ASSERT(!"Error: animation manager has not been initialize");
    }
    instance.Terminate();
    initialize = false;
}

AnimationManager* AnimationManager::Get()
{
    if (!initialize)
    {
        ASSERT(!"Error: animation manager has not been initialize");
    }
    return &instance;
}

void AnimationManager::Load(const char* name, const char* path, Model* model, int memoryType)
{
    if (!Contains(name))
    {

        void* buffer = MemoryManager::Get()->Alloc(sizeof(Animation), memoryType);
        Animation* animation = new (buffer) Animation();
        animation->Init(path, model, memoryType);

        AnimationHandle animationHandle{};
        animationHandle.name = name;
        animationHandle.animation = animation;

        AssetManager::Add(animationHandle, memoryType);
    }
}

void AnimationManager::Unload(AnimationHandle* handle)
{
}
Animation* AnimationManager::Get(const char* name)
{
    return AssetManager::Get(name)->animation;
}
