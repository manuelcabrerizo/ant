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
    instance.staticAllocator.Init(STATIC_MEMORY);
    instance.frameAllocator.Init(FRAME_MEMORY);
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

void SkeletonManager::ResetFrameAllocator()
{
    instance.frameAllocator.Init(FRAME_MEMORY);
}


void SkeletonManager::Load(const char* name, const char* path, int memoryType)
{
    if (ShouldLoad(name))
    {
        Skeleton* skeleton = nullptr;
        switch (memoryType)
        {
        case STATIC_MEMORY:
        {
            skeleton = staticAllocator.Alloc();
        } break;
        case FRAME_MEMORY: 
        {
            skeleton = frameAllocator.Alloc();
        } break;
        }
        skeleton->Init(path, memoryType);

        void* buffer = assets.Alloc();
        SkeletonHandle* skeletonHandle = new (buffer) SkeletonHandle();
        skeletonHandle->name = name;
        skeletonHandle->skeleton = skeleton;

        AssetManager::AssetRef assetRef;
        assetRef.refCount = 1;
        assetRef.asset = skeletonHandle;

        nameIndex.Add(name, assetRef);
    }
}

void SkeletonManager::Unload(const char* name)
{
    if (ShouldUnload(name))
    {
        SkeletonHandle* skeletonHandle = nameIndex.Get(name)->asset;
        Skeleton* skeleton = skeletonHandle->skeleton;
        
        switch (skeleton->GetMemoryType())
        {
        case STATIC_MEMORY:
        {
            staticAllocator.Free(skeleton);
        } break;
        case FRAME_MEMORY:
        {
            frameAllocator.Free(skeleton);
        } break;
        }

        nameIndex.Remove(name);
        assets.Free(skeletonHandle);
    }
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
    instance.staticAllocator.Init(STATIC_MEMORY);
    instance.frameAllocator.Init(FRAME_MEMORY);
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

void AnimationManager::ResetFrameAllocator()
{
    instance.frameAllocator.Init(FRAME_MEMORY);
}

void AnimationManager::Load(const char* name, const char* path, Model* model, int memoryType)
{
    if (ShouldLoad(name))
    {
        Animation* animation = nullptr;
        switch (memoryType)
        {
        case STATIC_MEMORY:
        {
            animation = staticAllocator.Alloc();
        } break;
        case FRAME_MEMORY:
        {
            animation = frameAllocator.Alloc();
        } break;
        }

        animation->Init(path, model, memoryType);

        void* buffer = assets.Alloc();
        AnimationHandle* animationHandle = new (buffer) AnimationHandle();
        animationHandle->name = name;
        animationHandle->animation = animation;

        AssetManager::AssetRef assetRef;
        assetRef.refCount = 1;
        assetRef.asset = animationHandle;

        nameIndex.Add(name, assetRef);
    }
}

void AnimationManager::Unload(const char* name)
{
    if (ShouldUnload(name))
    {
        AnimationHandle* animationHandle = nameIndex.Get(name)->asset;
        Animation* animation = animationHandle->animation;

        switch (animation->GetMemoryType())
        {
        case STATIC_MEMORY:
        {
            staticAllocator.Free(animation);
        } break;
        case FRAME_MEMORY:
        {
            frameAllocator.Free(animation);
        } break;
        }

        nameIndex.Remove(name);
        assets.Free(animationHandle);
    }
}

Animation* AnimationManager::Get(const char* name)
{
    return AssetManager::Get(name)->animation;
}