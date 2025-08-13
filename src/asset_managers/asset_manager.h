#pragma once

#include <containers.h>

template <typename Type>
class AssetManager
{
protected:
    // change slotmap for a BlockArray
     Slotmap<Type> assets;
     HashMap<SlotmapKey<Type>> nameIndex;
public:
     virtual ~AssetManager() {}
     
     virtual void Init(u32 assetsCapacity);
     void Terminate();
     void Clear();
     bool Contains(const char* name);

     virtual void Unload(const char *name) = 0;
protected:
     Type *Get(const char *name);
     Type *Get(SlotmapKey<Type> handle);
     SlotmapKey<Type> GetHandle(const char *name);
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
    for (i32 i = assets.GetArray()->size - 1; i >= 0; --i)
    {
        Type* asset = &assets.GetArray()->data[i];
        Unload(asset->name);
    }
}

template <typename Type>
Type* AssetManager<Type>::Get(const char* name)
{
    return assets.Get(*nameIndex.Get(name));
}

template <typename Type>
Type* AssetManager<Type>::Get(SlotmapKey<Type> handle)
{
    return assets.Get(handle);
}

template <typename Type>
SlotmapKey<Type> AssetManager<Type>::GetHandle(const char* name)
{
    return *nameIndex.Get(name);
}

template <typename Type>
bool AssetManager<Type>::Contains(const char* name)
{
    return nameIndex.Contains(name);
}