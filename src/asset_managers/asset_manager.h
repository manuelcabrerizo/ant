#pragma once

#include <containers.h>

template <typename Type>
class AssetManager
{
public:
    struct AssetRef
    {
        Type *asset = nullptr;
        int refCount = 0;
    };

protected:
    // change slotmap for a BlockArray
     BlockAllocator<sizeof(Type)> assets;
     HashMap<AssetRef> nameIndex;
public:
     virtual ~AssetManager() {}
     
     virtual void Init(u32 assetsCapacity);
     void Terminate();
     void Clear();
     bool Contains(const char* name);

     virtual void Unload(const char *name) = 0;
protected:
     Type *Get(const char *name);
     
     bool ShouldLoad(const char* name);
     bool ShouldUnload(const char* name);
};

template <typename Type>
void AssetManager<Type>::Init(u32 assetsCapacity)
{
    nameIndex.Init(assetsCapacity, STATIC_MEMORY);
    assets.Init(assetsCapacity, STATIC_MEMORY);
}

template <typename Type>
void AssetManager<Type>::Terminate()
{
    Clear();
}

template <typename Type>
void AssetManager<Type>::Clear()
{
    auto assetsArray = assets.GetBlockArray();
    size_t size = assets.GetBlockCount();
    size_t used = assets.GetBlockUsed();

    size_t usedFound = 0;
    for (int i = 0; i < size; ++i)
    {
        if (!assetsArray[i].header.occupied)
        {
            continue;
        }
        usedFound++;

        Type* asset = (Type*)&assetsArray[i].data[0];
        Unload(asset->name);

        if (usedFound == used)
        {
            break;
        }
    }
}

template <typename Type>
Type* AssetManager<Type>::Get(const char* name)
{
    return nameIndex.Get(name)->asset;
}

template <typename Type>
bool AssetManager<Type>::Contains(const char* name)
{
    return nameIndex.Contains(name);
}

template <typename Type>
bool AssetManager<Type>::ShouldLoad(const char* name)
{
    if (Contains(name))
    {
        // increment ref
        auto assetRef = nameIndex.Get(name);
        assetRef->refCount++;
        return false;
    }
    return true;
}

// TODO: return the handle to the asset to not repear the call to
// nameIndex.Get
template <typename Type>
bool AssetManager<Type>::ShouldUnload(const char* name)
{
    ASSERT(Contains(name));
    auto assetRef = nameIndex.Get(name);
    assetRef->refCount--;
    if (assetRef->refCount == 0)
    {
        return true;
    }
    return false;
}