#pragma once

#include <containers.h>

template <typename Type>
class AssetManager
{
protected:
     HashMap<Type> assetsMap;
     Array<Type*> staticAssetsList;
     Array<Type*> frameAssetsList;
public:
     virtual ~AssetManager() {}
     
     void Init(u32 assetsCapacity);
     void Terminate();

     void Clear(int memoryType);
     bool Contains(const char* name);
     Type* Get(const char* name);

protected:
    void Add(Type asset, int memoryType);
     virtual void Unload(Type* handle) = 0;
};

template <typename Type>
void AssetManager<Type>::Init(u32 assetsCapacity)
{
    assetsMap.Init(assetsCapacity, STATIC_MEMORY);
    staticAssetsList.Init(assetsCapacity, STATIC_MEMORY);
    frameAssetsList.Init(assetsCapacity, STATIC_MEMORY);
}

template <typename Type>
void AssetManager<Type>::Terminate()
{
    Clear(FRAME_MEMORY);
    Clear(STATIC_MEMORY);
}

template <typename Type>
void AssetManager<Type>::Clear(int memoryType)
{
    switch (memoryType)
    {
    case FRAME_MEMORY:
    {
        for (int i = 0; i < frameAssetsList.size; i++)
        {
            Unload(frameAssetsList[i]);
            assetsMap.Remove(frameAssetsList[i]->name);
        }
        frameAssetsList.Clear();
    } break;
    case STATIC_MEMORY:
    {
        for (int i = 0; i < staticAssetsList.size; i++)
        {
            Unload(staticAssetsList[i]);
            assetsMap.Remove(staticAssetsList[i]->name);
        }
        staticAssetsList.Clear();
    } break;
    }
}

template <typename Type>
Type* AssetManager<Type>::Get(const char* name)
{
    return assetsMap.Get(name);
}

template <typename Type>
bool AssetManager<Type>::Contains(const char* name)
{
    return assetsMap.Contains(name);
}

template <typename Type>
void AssetManager<Type>::Add(Type asset, int memoryType)
{
    assetsMap.Add(asset.name, asset);
    switch (memoryType)
    {
    case FRAME_MEMORY:
    {
        frameAssetsList.Push(assetsMap.Get(asset.name));
    } break;
    case STATIC_MEMORY:
    {
        staticAssetsList.Push(assetsMap.Get(asset.name));
    } break;
    }
}
