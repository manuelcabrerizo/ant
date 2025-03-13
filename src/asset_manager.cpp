template <typename Type>
void AssetManager<Type>::Init(u32 assetsCapacity)
{
     nameIndex.Init(assetsCapacity * sizeof(Type), STATIC_MEMORY);
     assets.Init(assetsCapacity, STATIC_MEMORY);
}

template <typename Type>
void AssetManager<Type>::Terminate()
{
     // TODO: test this and think other ways ...
     Frame frame = MemoryManager::Get()->GetFrame(FRAME_MEMORY);
     
     Array<Type> assetsArray;
     u32 size = assets.GetArray()->size;
     assetsArray.Init(size, FRAME_MEMORY);
     memcpy(assetsArray.data, assets.GetArray()->data, sizeof(Type) * size);
     
     for(u32 i = 0; i < size; ++i)
     {
          Type *asset = &assetsArray[i];
          Unload(asset->name);
     }

     MemoryManager::Get()->ReleaseFrame(frame);
}
     
template <typename Type>
Type *AssetManager<Type>::Get(const char *name)
{
     return assets.Get(*nameIndex.Get(name));
}

template <typename Type>
Type *AssetManager<Type>::Get(SlotmapKey<Type> handle)
{
     return assets.Get(handle);
}

template <typename Type>
SlotmapKey<Type> AssetManager<Type>::GetHandle(const char *name)
{
     return *nameIndex.Get(name);
}


// Texture Manager
void TextureManager::Load(const char *name)
{
     TextureHandle textureHandle;
     textureHandle.name = name;
     textureHandle.texture = GraphicsManager::Get()->TextureAlloc(name);
     nameIndex.Add(name, assets.Add(textureHandle));
}

void TextureManager::Unload(const char *name)
{
     auto handle = *nameIndex.Get(name);
     GraphicsManager::Get()->TextureFree(assets.Get(handle)->texture);
     assets.Remove(handle);
     nameIndex.Remove(name);
}

// Model Manager
void ModelManager::Load(const char *name)
{
     ModelHandle modelHandle;
     modelHandle.name = name;
     modelHandle.model.Init(name);
     nameIndex.Add(name, assets.Add(modelHandle));
}

void ModelManager::Unload(const char *name)
{
     auto handle = *nameIndex.Get(name);
     assets.Get(handle)->model.Terminate();
     assets.Remove(handle);
     nameIndex.Remove(name);
}
